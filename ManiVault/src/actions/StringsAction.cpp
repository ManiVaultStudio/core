// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StringsAction.h"
#include "Application.h"

#include <QHBoxLayout>
#include <QStandardItemModel>

using namespace mv::util;

namespace mv::gui {

StringsAction::StringsAction(QObject* parent, const QString& title, const QStringList& strings /*= QStringList()*/) :
    WidgetAction(parent, title),
    _category("String"),
    _toolbarAction(this, "Toolbar"),
    _nameAction(&_toolbarAction, "Name"),
    _addAction(&_toolbarAction, "Add"),
    _removeAction(&_toolbarAction, "Remove"),
    _allowDuplicates(true)
{
    setText(title);
    setDefaultWidgetFlags(WidgetFlag::Default);
    setStrings(strings);

    _toolbarAction.setShowLabels(false);

    _nameAction.setEnabled(true);
    _nameAction.setStretch(1);

    _addAction.setEnabled(false);

    _removeAction.setEnabled(false);

    _toolbarAction.addAction(&_nameAction);
    _toolbarAction.addAction(&_addAction);
    _toolbarAction.addAction(&_removeAction);
}

QStringList StringsAction::getStrings() const
{
    return _stringsModel.stringList();
}

void StringsAction::setStrings(const QStringList& strings)
{
    if (strings == getStrings())
        return;

    _stringsModel.setStringList(strings);

    emit stringsChanged(getStrings());

    auto removedStrings = getStrings();
    auto addedStrings   = strings;

    for (const auto& item : strings) {
        removedStrings.removeAll(item);
    }

    for (const QString& item : getStrings()) {
        addedStrings.removeAll(item);
    }

    if (!addedStrings.isEmpty())
        emit stringsAdded(addedStrings);

    if (!removedStrings.isEmpty())
        emit stringsRemoved(removedStrings);

    saveToSettings();
}

QStringList StringsAction::getLockedStrings() const
{
    return _lockedStrings;
}

void StringsAction::setLockedStrings(const QStringList& lockedStrings)
{
    if (lockedStrings == _lockedStrings)
        return;

    _lockedStrings = lockedStrings;

    emit lockedStringsChanged(_lockedStrings);
}

bool StringsAction::allowsDuplicates() const
{
    return _allowDuplicates;
}

void StringsAction::setAllowDuplicates(bool allowDuplicates)
{
    if (allowDuplicates == _allowDuplicates)
        return;

    _allowDuplicates = allowDuplicates;

    emit allowDuplicatesChanged(_allowDuplicates);
}

bool StringsAction::mayAddString(const QString& string) const
{
    if (string.isEmpty())
        return false;

	if (allowsDuplicates())
		return true;

	return !getStrings().contains(string);
}

void StringsAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicStringsAction = dynamic_cast<StringsAction*>(publicAction);

    Q_ASSERT(publicStringsAction != nullptr);

    if (publicStringsAction == nullptr)
        return;

    connect(this, &StringsAction::stringsChanged, publicStringsAction, &StringsAction::setStrings);
    connect(publicStringsAction, &StringsAction::stringsChanged, this, &StringsAction::setStrings);

    setStrings(publicStringsAction->getStrings());

    WidgetAction::connectToPublicAction(publicAction, recursive);
}

void StringsAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    auto publicStringsAction = dynamic_cast<StringsAction*>(getPublicAction());

    Q_ASSERT(publicStringsAction != nullptr);

    if (publicStringsAction == nullptr)
        return;

    disconnect(this, &StringsAction::stringsChanged, publicStringsAction, &StringsAction::setStrings);
    disconnect(publicStringsAction, &StringsAction::stringsChanged, this, &StringsAction::setStrings);

    WidgetAction::disconnectFromPublicAction(recursive);
}

void StringsAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "Value");

    setStrings(variantMap["Value"].toStringList());
}

QVariantMap StringsAction::toVariantMap() const
{
    auto variantMap = WidgetAction::toVariantMap();

    variantMap.insert({
        { "Value", QVariant::fromValue(getStrings()) }
    });

    return variantMap;
}

QString StringsAction::getCategory() const
{
    return _category;
}

void StringsAction::setCategory(const QString& category)
{
    _category = category;
}

void StringsAction::addString(const QString& string)
{
    addStrings({ string });
}

void StringsAction::addStrings(const QStringList& strings, bool allowDuplication /*= false*/)
{
    if (strings.isEmpty())
        return;

    const auto previousStrings = getStrings();

    auto updatedStrings = getStrings();

    updatedStrings << strings;

	if (!allowDuplication) {
        updatedStrings.removeDuplicates();
    }

    getStringsModel().setStringList(updatedStrings);

    auto addedStrings = strings;

    for (const auto& string : previousStrings)
        addedStrings.removeAll(string);

    if (!addedStrings.isEmpty()) {
        emit stringsAdded(addedStrings);
        emit stringsChanged(getStrings());
    }

    saveToSettings();
}

void StringsAction::removeString(const QString& string)
{
    removeStrings({ string });
}

void StringsAction::removeStrings(const QStringList& strings)
{
    auto updatedStrings = getStrings();

    QStringList removedStrings;

    for (const auto& string : strings)
        if (auto numberRemoved = updatedStrings.removeAll(string); numberRemoved > 0)
            removedStrings << string;

    getStringsModel().setStringList(updatedStrings);
    
    if (!removedStrings.isEmpty()) {
	    emit stringsChanged(updatedStrings);
    	emit stringsRemoved(removedStrings);
    }
}

void StringsAction::updateString(const QString& oldString, const QString& newString)
{
    Q_ASSERT(!oldString.isEmpty() && !newString.isEmpty());

    if (oldString.isEmpty()) {
        qWarning() << "Cannot update string because old string is empty";
        return;
    }

    if (newString.isEmpty()) {
        qWarning() << "Cannot update string because new string is empty";
        return;
    }

    if (newString == oldString) {
        qWarning() << "Cannot update string from" << oldString << "to" << newString << "because they are the same";
        return;
    }

    if (!getStrings().contains(oldString)) {
        qWarning() << "Cannot update" << oldString << "to" << newString << "because it does not exist in the list";
        return;
    }

    const auto previousStrings = getStrings();

    auto updatedStrings = getStrings();

    updatedStrings.replaceInStrings(oldString, newString);

    if (updatedStrings != previousStrings) {
        getStringsModel().setStringList(updatedStrings);

        emit stringsChanged(getStrings());
        emit stringUpdated(oldString, newString);
    }
}

QWidget* StringsAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    if (widgetFlags & WidgetFlag::ListView)
        layout->addWidget(new ListWidget(parent, this, widgetFlags));

    widget->setLayout(layout);

    return widget;
}

StringsAction::ListWidget::ListWidget(QWidget* parent, StringsAction* stringsAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, stringsAction, widgetFlags),
    _stringsAction(stringsAction),
    _hierarchyWidget(this, stringsAction->getCategory(), stringsAction->getStringsModel(), nullptr, false)
{
    resize(0, 150);

    _hierarchyWidget.getTreeView().setRootIsDecorated(false);
    _hierarchyWidget.getTreeView().setAnimated(true);
    _hierarchyWidget.setWindowIcon(stringsAction->icon());
    _hierarchyWidget.setHeaderHidden(true);
    
    _filterModel.setFilterKeyColumn(0);
    
    auto& filterGroupAction = _hierarchyWidget.getFilterGroupAction();

    filterGroupAction.setLabelWidthFixed(40);
    filterGroupAction.setPopupSizeHint(QSize(350, 0));

    _stringsAction->getNameAction().setClearable(true);
    _stringsAction->getNameAction().setPlaceHolderString(QString("Enter %1 here...").arg(_stringsAction->getCategory().toLower()));

    _stringsAction->getAddAction().setIconByName("plus");
    _stringsAction->getAddAction().setToolTip(QString("Add entered %1").arg(_stringsAction->getCategory().toLower()));
    _stringsAction->getAddAction().setDefaultWidgetFlags(TriggerAction::Icon);

    _stringsAction->getRemoveAction().setIconByName("trash");
    _stringsAction->getRemoveAction().setToolTip(QString("Remove selected %1(s)").arg(_stringsAction->getCategory().toLower()));
    _stringsAction->getRemoveAction().setDefaultWidgetFlags(TriggerAction::Icon);

    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(&_hierarchyWidget, 1);

    if (widgetFlags & StringsAction::WidgetFlag::MayEdit)
        layout->addWidget(_stringsAction->getToolbarAction().createWidget(this));

    setLayout(layout);

    const auto isStringLocked = [this](const QString& string) -> bool {
        return _stringsAction->getLockedStrings().contains(string);
    };

    const auto clearSelection = [this]() -> void {
        _hierarchyWidget.getSelectionModel().clear();

        _selectedRows = QModelIndexList();
	};
    
    const auto updateNameAction = [this, isStringLocked]() -> void {
        const auto numberOfSelectedRows = _selectedRows.count();

        if (numberOfSelectedRows == 0) {
            _stringsAction->getNameAction().setEnabled(true);
            _stringsAction->getNameAction().setString("");
        }

        if (numberOfSelectedRows == 1) {
            _stringsAction->getNameAction().setEnabled(false);
            _stringsAction->getNameAction().setString("");
        }

        if (numberOfSelectedRows >= 2) {
            _stringsAction->getNameAction().setEnabled(false);
            _stringsAction->getNameAction().setString("");
        }
    };

    const auto updateAddAction = [this, isStringLocked]() -> void {
        const auto numberOfSelectedRows = _selectedRows.count();

        if (numberOfSelectedRows == 0)
            _stringsAction->getAddAction().setEnabled(_stringsAction->mayAddString(_stringsAction->getNameAction().getString()));

        if (numberOfSelectedRows >= 1)
            _stringsAction->getAddAction().setEnabled(false);
	};

    connect(&_stringsAction->getNameAction(), &StringAction::stringChanged, this, updateAddAction);

    const auto updateRemoveAction = [this, isStringLocked]() -> void {
        const auto numberOfSelectedRows = _selectedRows.count();

        if (numberOfSelectedRows == 0)
            _stringsAction->getRemoveAction().setEnabled(false);

        if (numberOfSelectedRows == 1)
            _stringsAction->getRemoveAction().setEnabled(true);

        if (numberOfSelectedRows >= 2) {
            bool mayRemove = false;

            for (const auto& selectedRow : _selectedRows)
                if (!isStringLocked(selectedRow.data().toString()))
                    mayRemove = true;

            _stringsAction->getRemoveAction().setEnabled(mayRemove);
        }
    };


    connect(&_stringsAction->getStringsModel(), &QStringListModel::dataChanged, this, [this](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<int>& roles) {
        if (topLeft.column() != 0 || bottomRight.column() != 0)
            return;

        if (!roles.isEmpty() && !roles.contains(Qt::EditRole))
            return;

        auto& stringsModel = _stringsAction->getStringsModel();

        for (int rowIndex = topLeft.row(); rowIndex <= bottomRight.row(); ++rowIndex) {
            const auto oldString = _strings.at(rowIndex);
            const auto newString = stringsModel.data(stringsModel.index(rowIndex, 0), Qt::EditRole).toString();

            _stringsAction->updateString(oldString, newString);
        }
    });

    connect(_stringsAction, &StringsAction::stringsChanged, this, [this, updateNameAction, updateAddAction, updateRemoveAction]() -> void {
        _filterModel.invalidate();

        _hierarchyWidget.getSelectionModel().clear();

        _strings = _stringsAction->getStrings();

        updateNameAction();
        updateAddAction();
        updateRemoveAction();
	});

    connect(_stringsAction, &StringsAction::lockedStringsChanged, this, [this, updateNameAction, updateAddAction, updateRemoveAction]() -> void {
        _hierarchyWidget.getSelectionModel().clear();

    	updateNameAction();
        updateAddAction();
        updateRemoveAction();
	});

    connect(&_stringsAction->getNameAction(), &StringAction::stringChanged, this, [this, isStringLocked]() -> void {
        if (_selectedRows.isEmpty())
            return;

        const auto selectedName     = _selectedRows.first().data(Qt::DisplayRole).toString();
        const auto targetName       = _stringsAction->getNameAction().getString();
        const auto stringIsLocked   = isStringLocked(targetName);

        if (_selectedRows.count() == 1 && !stringIsLocked){
            _stringsAction->getNameAction().setEnabled(!stringIsLocked);
            _stringsAction->updateString(selectedName, targetName);
        }
    });

    connect(&_stringsAction->getAddAction(), &TriggerAction::triggered, this, [this]() -> void {
        if (_stringsAction->getNameAction().getString().isEmpty())
            return;

        _stringsAction->addString(_stringsAction->getNameAction().getString());
        _stringsAction->getNameAction().setString("");
    });

    connect(&_stringsAction->getRemoveAction(), &TriggerAction::triggered, this, [this, clearSelection]() -> void {
        QStringList stringsToRemove;

        for (const auto& selectedRow : _hierarchyWidget.getSelectionModel().selectedRows())
            stringsToRemove << selectedRow.data(Qt::DisplayRole).toString();

        clearSelection();

        _stringsAction->removeStrings(stringsToRemove);
        _stringsAction->getNameAction().setString("");
    });

    connect(&_hierarchyWidget.getSelectionModel(), &QItemSelectionModel::selectionChanged, this, [this, updateNameAction, updateAddAction, updateRemoveAction](const QItemSelection& selected, const QItemSelection& deselected) -> void {
        _selectedRows = _hierarchyWidget.getSelectionModel().selectedRows();

        updateNameAction();
        updateAddAction();
        updateRemoveAction();
    });

    updateNameAction();
    updateAddAction();
    updateRemoveAction();
}

}
