// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "OptionsAction.h"
#include "Application.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QStylePainter>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QAbstractItemView>
#include <QMouseEvent>
#include <QListView>
#include <QAbstractItemView>

using namespace mv::util;

namespace mv::gui {

OptionsAction::OptionsAction(QObject* parent, const QString& title, const QStringList& options /*= QStringList()*/, const QStringList& selectedOptions /*= QStringList()*/) :
    WidgetAction(parent, title),
    _optionsModel({}),
    _selectionAction(*this),
    _fileAction(*this)
{
    setText(title);
    setDefaultWidgetFlags(WidgetFlag::Default);
    initialize(options, selectedOptions);

    connect(&_optionsModel, &QStandardItemModel::dataChanged, this, [this]() -> void {
        //selectedOptionsChanged(getSelectedOptions());
    });
}

void OptionsAction::initialize(const QStringList& options /*= QStringList()*/, const QStringList& selectedOptions /*= QStringList()*/)
{
    setOptions(options);
    setSelectedOptions(selectedOptions);
}

QStringList OptionsAction::getOptions() const
{
    return _optionsModel.stringList();
}

const OptionsAction::OptionsStringListModel& OptionsAction::getOptionsModel() const
{
    return _optionsModel;
}

OptionsAction::OptionsStringListModel& OptionsAction::getOptionsModel()
{
    return _optionsModel;
}

std::uint32_t OptionsAction::getNumberOfOptions() const
{
    return _optionsModel.rowCount();
}

bool OptionsAction::hasOption(const QString& option) const
{
    return _optionsModel.match(_optionsModel.index(0, 0), Qt::DisplayRole, option).count() == 1;
}

bool OptionsAction::hasOptions() const
{
    return _optionsModel.rowCount() >= 1;
}

void OptionsAction::setOptions(const QStringList& options, bool clearSelection /*= false*/)
{
    const auto selectedOptions = getSelectedOptions();

    _optionsModel.setStringList(options);

    for (std::int32_t rowIndex = 0; rowIndex < options.count(); ++rowIndex) {

        auto index = _optionsModel.index(rowIndex, 0, QModelIndex());

        _optionsModel.setData(index, clearSelection ? Qt::Unchecked : (selectedOptions.contains(options.at(rowIndex)) ? Qt::Checked : Qt::Unchecked), Qt::CheckStateRole);
    }

    emit optionsChanged(getOptions());
}

QStringList OptionsAction::getSelectedOptions() const
{
    QStringList selectedOptions;

    for (std::int32_t optionIndex = 0; optionIndex < _optionsModel.rowCount(); optionIndex++) {
        const auto index = _optionsModel.index(optionIndex, 0);

        if (index.data(Qt::CheckStateRole).toInt() == Qt::Checked)
            selectedOptions << index.data(Qt::EditRole).toString();
    }

    return selectedOptions;
}

QList<std::int32_t> OptionsAction::getSelectedOptionIndices() const
{
    QList<std::int32_t> selectedOptionIndices;

    for (std::int32_t optionIndex = 0; optionIndex < _optionsModel.rowCount(); optionIndex++)
        if (_optionsModel.index(optionIndex, 0).data(Qt::CheckStateRole).toInt() == Qt::Checked)
            selectedOptionIndices << optionIndex;

    return selectedOptionIndices;
}

bool OptionsAction::isOptionSelected(const QString& option) const
{
    const auto matches = _optionsModel.match(_optionsModel.index(0, 0), Qt::DisplayRole, option);

    if (matches.isEmpty())
        return false;

    return matches.first().data(Qt::CheckStateRole).toInt() == Qt::Checked;
}

bool OptionsAction::hasSelectedOptions() const
{
    for (std::int32_t optionIndex = 0; optionIndex < _optionsModel.rowCount(); optionIndex++)
        if (_optionsModel.index(optionIndex, 0).data(Qt::CheckStateRole).toInt() == Qt::Checked)
            return true;

    return false;
}

void OptionsAction::selectOption(const QString& option, bool unselect /*= false*/)
{
    const auto matches = _optionsModel.match(_optionsModel.index(0, 0), Qt::DisplayRole, option);

    if (!matches.isEmpty())
        _optionsModel.setData(matches.first(), unselect ? Qt::Unchecked : Qt::Checked, Qt::CheckStateRole);

    saveToSettings();
}

void OptionsAction::setSelectedOptions(const QStringList& selectedOptions)
{
    if (selectedOptions == getSelectedOptions())
        return;

    auto previousSelectedOptions = getSelectedOptions();

    QSignalBlocker optionsModelBlocker(&_optionsModel);

    auto selectionChanged = false;

    for (std::int32_t optionIndex = 0; optionIndex < _optionsModel.rowCount(); optionIndex++)
        _optionsModel.setData(_optionsModel.index(optionIndex, 0), selectedOptions.contains(_optionsModel.index(optionIndex, 0).data(Qt::EditRole).toString()) ? Qt::Checked : Qt::Unchecked, Qt::CheckStateRole);

    if (getSelectedOptions() != previousSelectedOptions)
        emit selectedOptionsChanged(getSelectedOptions());

    saveToSettings();
}

void OptionsAction::connectToPublicAction(WidgetAction* publicAction, bool recursive /*= true*/)
{
    auto publicOptionsAction = dynamic_cast<OptionsAction*>(publicAction);

    Q_ASSERT(publicOptionsAction != nullptr);

    if (publicOptionsAction == nullptr)
        return;

    const auto updatePublicOptions = [this, publicOptionsAction]() -> void {
        auto allOptions = publicOptionsAction->getOptions() + getOptions();

        allOptions.removeDuplicates();
        allOptions.sort();

        publicOptionsAction->setOptions(allOptions);
    };

    updatePublicOptions();

    const auto initializeSelectedOptions = [this, publicOptionsAction]() -> void {
        auto publicSelectedOptions = publicOptionsAction->getSelectedOptions();

        for (auto selectedOption : getSelectedOptions())
            publicSelectedOptions << selectedOption;

        setSelectedOptions(publicSelectedOptions);
    };

    initializeSelectedOptions();

    const auto initializePublicOptionsSelection = [this, publicOptionsAction]() -> void {
        auto publicSelectedOptions = publicOptionsAction->getSelectedOptions();

        for (auto selectedOption : getSelectedOptions())
            publicSelectedOptions << selectedOption;

        publicOptionsAction->setSelectedOptions(publicSelectedOptions);
    };

    initializePublicOptionsSelection();

    connect(this, &OptionsAction::selectedOptionsChanged, this, [this, publicOptionsAction]() -> void {
        auto publicSelectedOptions = publicOptionsAction->getSelectedOptions();

        for (auto option : getOptions())
            publicSelectedOptions.removeOne(option);

        for (auto selectedOption : getSelectedOptions())
            publicSelectedOptions << selectedOption;

        publicOptionsAction->setSelectedOptions(publicSelectedOptions);
    });

    connect(publicOptionsAction, &OptionsAction::selectedOptionsChanged, this, [this, publicOptionsAction, initializePublicOptionsSelection](const QStringList& selectedOptions) -> void {
        setSelectedOptions(selectedOptions);
    });

    connect(this, &OptionsAction::optionsChanged, this, updatePublicOptions);

    WidgetAction::connectToPublicAction(publicAction, recursive);
}

void OptionsAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    auto publicOptionsAction = dynamic_cast<OptionsAction*>(getPublicAction());

    Q_ASSERT(publicOptionsAction != nullptr);

    if (publicOptionsAction == nullptr)
        return;

    disconnect(this, &OptionsAction::selectedOptionsChanged, this, nullptr);
    disconnect(publicOptionsAction, &OptionsAction::selectedOptionsChanged, this, nullptr);
    disconnect(this, &OptionsAction::optionsChanged, this, nullptr);

    WidgetAction::disconnectFromPublicAction(recursive);
}

void OptionsAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "Value");
    variantMapMustContain(variantMap, "IsPublic");

    if (variantMap["IsPublic"].toBool())
        setOptions(variantMap["Options"].toStringList());

    setSelectedOptions(variantMap["Value"].toStringList());
}

QVariantMap OptionsAction::toVariantMap() const
{
    auto variantMap = WidgetAction::toVariantMap();

    variantMap.insert({
        { "Value", getSelectedOptions() }
    });

    if (isPublic()) {
        variantMap.insert({
            { "Options", getOptions() }
        });
    }

    return variantMap;
}

class CheckableItemView : public QAbstractItemView {
public:
    CheckableItemView(QWidget* parent = nullptr) : QAbstractItemView(parent) {}

protected:
    QModelIndex indexAt(const QPoint& point) const override {
        Q_UNUSED(point)
            return QModelIndex();
    }

    void scrollTo(const QModelIndex& index, ScrollHint hint = EnsureVisible) override {
        Q_UNUSED(index)
            Q_UNUSED(hint)
    }

    QRect visualRect(const QModelIndex& index) const override {
        Q_UNUSED(index)
            return QRect();
    }

    QModelIndex moveCursor(QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers) override {
        Q_UNUSED(cursorAction)
            Q_UNUSED(modifiers)
            return QModelIndex();
    }

    int horizontalOffset() const override {
        return 0;
    }

    int verticalOffset() const override {
        return 0;
    }

    bool isIndexHidden(const QModelIndex& index) const override {
        Q_UNUSED(index)
            return false;
    }

    void setSelection(const QRect& rect, QItemSelectionModel::SelectionFlags flags) override {
        Q_UNUSED(rect)
            Q_UNUSED(flags)
    }

    QRegion visualRegionForSelection(const QItemSelection& selection) const override {
        Q_UNUSED(selection)
            return QRegion();
    }

    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) {
            QModelIndex index = indexAt(event->pos());
            if (index.isValid()) {
                QAbstractItemModel* model = this->model();
                if (model) {
                    QVariant value = model->data(index, Qt::CheckStateRole);
                    if (value.isValid()) {
                        Qt::CheckState state = static_cast<Qt::CheckState>(value.toInt());
                        model->setData(index, (state == Qt::Unchecked) ? Qt::Checked : Qt::Unchecked, Qt::CheckStateRole);
                        event->accept();
                        return;
                    }
                }
            }
        }
        QAbstractItemView::mousePressEvent(event);
    }
};

class CheckableCompleter : public QCompleter {
public:
    CheckableCompleter(QObject* parent = nullptr) : QCompleter(parent) {}

    bool event(QEvent* event) override {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                QModelIndex index = popup()->indexAt(mouseEvent->pos());
                if (index.isValid()) {
                    QVariant value = completionModel()->data(index, Qt::CheckStateRole);
                    if (value.isValid()) {
                        Qt::CheckState state = static_cast<Qt::CheckState>(value.toInt());
                        completionModel()->setData(index, (state == Qt::Unchecked) ? Qt::Checked : Qt::Unchecked, Qt::CheckStateRole);
                        return true;
                    }
                }
            }
        }
        return QCompleter::event(event);
    }
};

class CustomCompleter : public QCompleter {
public:
    using QCompleter::QCompleter;

protected:
    QAbstractItemView* popup() {
        auto listView = new QListView();

        listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        listView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        listView->setSelectionBehavior(QAbstractItemView::SelectRows);
        listView->setSelectionMode(QAbstractItemView::MultiSelection);
        listView->setAlternatingRowColors(true);

        return listView;
    }
};

OptionsAction::ComboBoxWidget::ComboBoxWidget(QWidget* parent, OptionsAction* optionsAction) :
    QComboBox(parent),
    _optionsAction(optionsAction),
    _completer()
{
    auto completer = new CustomCompleter();

    setObjectName("ComboBox");
    setEditable(true);
    setCompleter(completer);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setSizeAdjustPolicy(QComboBox::AdjustToContents);
    setModel(&optionsAction->getOptionsModel());
    setModelColumn(0);
    setInsertPolicy(QComboBox::NoInsert);

    //auto listView = new QListView();

    //listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //listView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //listView->setSelectionBehavior(QAbstractItemView::SelectRows);
    //listView->setSelectionMode(QAbstractItemView::MultiSelection);
    //listView->setAlternatingRowColors(true);

    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    completer->setCompletionColumn(0);
    completer->setCompletionMode(QCompleter::PopupCompletion);

    auto listView = new QListView();

    listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    listView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listView->setSelectionBehavior(QAbstractItemView::SelectRows);
    listView->setSelectionMode(QAbstractItemView::MultiSelection);
    listView->setAlternatingRowColors(true);

    completer->setPopup(listView);
    completer->setModel(&optionsAction->getOptionsModel());

    const auto onSelectedOptionsChanged = [this, optionsAction]() -> void {
        setToolTip("Selected: " + optionsAction->getSelectedOptions().join(", "));
        update();
    };

    connect(optionsAction, &OptionsAction::selectedOptionsChanged, this, onSelectedOptionsChanged);

    onSelectedOptionsChanged();
}

void OptionsAction::ComboBoxWidget::paintEvent(QPaintEvent* paintEvent)
{
    QComboBox::paintEvent(paintEvent);
    return;
    auto painter = QSharedPointer<QStylePainter>::create(this);

    painter->setPen(palette().color(QPalette::Text));

    auto styleOptionComboBox = QStyleOptionComboBox();

    initStyleOption(&styleOptionComboBox);

    painter->drawComplexControl(QStyle::CC_ComboBox, styleOptionComboBox);

    styleOptionComboBox.palette.setBrush(QPalette::ButtonText, styleOptionComboBox.palette.brush(QPalette::ButtonText).color().lighter());

    const auto selectedOptions = _optionsAction->getSelectedOptions();

    styleOptionComboBox.currentText = selectedOptions.isEmpty() ? "None selected" : selectedOptions.join(", ");

    painter->drawControl(QStyle::CE_ComboBoxLabel, styleOptionComboBox);
}

QWidget* OptionsAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    auto widget = new WidgetActionWidget(parent, this, widgetFlags);
    auto layout = new QHBoxLayout();

    if (!widget->isPopup())
        layout->setContentsMargins(0, 0, 0, 0);

    if (widgetFlags & WidgetFlag::ComboBox)
        layout->addWidget(new OptionsAction::ComboBoxWidget(parent, this));

    if (widgetFlags & WidgetFlag::Selection)
        layout->addWidget(_selectionAction.createCollapsedWidget(parent));

    if (widgetFlags & WidgetFlag::File)
        layout->addWidget(_fileAction.createCollapsedWidget(parent));

    widget->setLayout(layout);

    return widget;
}

OptionsAction::SelectionAction::SelectionAction(OptionsAction& optionsAction) :
    WidgetAction(&optionsAction, "Selection"),
    _optionsAction(optionsAction),
    _selectAllAction(this, "All"),
    _clearSelectionAction(this, "Clear"),
    _invertSelectionAction(this, "Invert")
{
    auto& fontAwesome = Application::getIconFont("FontAwesome");

    setText("Selection");
    setToolTip("Change selection");
    setIconByName("mouse-pointer");

    const auto updateReadOnly = [this]() -> void {
        _selectAllAction.setEnabled(_optionsAction.getSelectedOptions().count() < _optionsAction.getOptions().count());
        _clearSelectionAction.setEnabled(_optionsAction.getSelectedOptions().count() >= 1);
    };

    connect(&_optionsAction, &OptionsAction::selectedOptionsChanged, this, updateReadOnly);

    connect(&_selectAllAction, &TriggerAction::triggered, this, [this]() -> void {
        _optionsAction.setSelectedOptions(_optionsAction.getOptions());
    });

    connect(&_clearSelectionAction, &TriggerAction::triggered, this, [this]() -> void {
        _optionsAction.setSelectedOptions(QStringList());
    });

    connect(&_invertSelectionAction, &TriggerAction::triggered, this, [this]() -> void {
        auto invertedSelectedOptions = _optionsAction.getOptions();

        for (const auto& selectedOption : _optionsAction.getSelectedOptions())
            invertedSelectedOptions.removeOne(selectedOption);

        _optionsAction.setSelectedOptions(invertedSelectedOptions);
    });
}

OptionsAction::SelectionAction::Widget::Widget(QWidget* parent, SelectionAction* selectionAction) :
    WidgetActionWidget(parent, selectionAction)
{
    auto layout = new QHBoxLayout();

    layout->addWidget(selectionAction->getSelectAllAction().createWidget(this));
    layout->addWidget(selectionAction->getClearSelectionAction().createWidget(this));
    layout->addWidget(selectionAction->getInvertSelectionAction().createWidget(this));

    setLayout(layout);
}

OptionsAction::FileAction::FileAction(OptionsAction& optionsAction) :
    WidgetAction(&optionsAction, "File"),
    _optionsAction(optionsAction),
    _loadSelectionAction(this, "Load selection"),
    _saveSelectionAction(this, "Save selection")
{
    setText("File");
    setToolTip("Load/save selection");
    setIconByName("file");

    const auto updateReadOnly = [this]() -> void {
        _loadSelectionAction.setEnabled(_optionsAction.getOptions().count() >= 1);
        _saveSelectionAction.setEnabled(_optionsAction.getOptions().count() >= 1);
    };

    connect(&_optionsAction, &OptionsAction::selectedOptionsChanged, this, updateReadOnly);

    connect(&_loadSelectionAction, &TriggerAction::triggered, this, [this]() -> void {
        auto* fileDialog = new QFileDialog();

        fileDialog->setWindowTitle("Open selection from file");
        fileDialog->setWindowIcon(Application::getIconFont("FontAwesome").getIcon("folder-open"));
        fileDialog->setAcceptMode(QFileDialog::AcceptOpen);
        fileDialog->setFileMode(QFileDialog::ExistingFile);
        fileDialog->setNameFilters({ "Selection files (*.json)" });
        fileDialog->setDefaultSuffix(".json");
        //fileDialog->setDirectory(ApplicationgetSetting("Projects/WorkingDirectory", QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)).toString());

        connect(fileDialog, &QFileDialog::accepted, this, [this, fileDialog]() -> void {
            if (fileDialog->selectedFiles().count() != 1)
                throw std::runtime_error("Only one file may be selected");

            QFile jsonFile(fileDialog->selectedFiles().first());

            jsonFile.open(QFile::ReadOnly);

            const auto jsonDocument = QJsonDocument::fromJson(jsonFile.readAll());
            const auto variantMap = jsonDocument.toVariant().toMap();
            const auto selection = variantMap["selection"].toStringList();

            _optionsAction.setSelectedOptions(selection);

            _loadSelectionAction.setEnabled(false);
            });
        connect(fileDialog, &QFileDialog::finished, fileDialog, &QFileDialog::deleteLater);

        fileDialog->open();

    });

    connect(&_saveSelectionAction, &TriggerAction::triggered, this, [this]() -> void {
        auto* fileDialog = new QFileDialog();

        fileDialog->setWindowTitle("Save selection to file");
        fileDialog->setWindowIcon(Application::getIconFont("FontAwesome").getIcon("save"));
        fileDialog->setAcceptMode(QFileDialog::AcceptSave);
        fileDialog->setNameFilters({ "Selection files (*.json)" });
        fileDialog->setDefaultSuffix(".json");
        fileDialog->setOption(QFileDialog::DontUseNativeDialog, true);
        //fileDialog->setDirectory(getSetting("Projects/WorkingDirectory", QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)).toString());

        connect(fileDialog, &QFileDialog::accepted, this, [this, fileDialog]() -> void {
            if (fileDialog->selectedFiles().count() != 1)
                throw std::runtime_error("Only one file may be selected");

            QJsonDocument jsonDocument;

            QJsonObject jsonObject;

            jsonObject.insert("selection", QJsonArray::fromStringList(_optionsAction.getSelectedOptions()));

            jsonDocument.setObject(jsonObject);

            QFile jsonFile(fileDialog->selectedFiles().first());

            jsonFile.open(QFile::WriteOnly);
            jsonFile.write(jsonDocument.toJson());

            _saveSelectionAction.setEnabled(false);
            });
        connect(fileDialog, &QFileDialog::finished, fileDialog, &QFileDialog::deleteLater);

        fileDialog->open();

    });

    updateReadOnly();
}

OptionsAction::FileAction::Widget::Widget(QWidget* parent, FileAction* fileAction) :
    WidgetActionWidget(parent, fileAction)
{
    auto layout = new QHBoxLayout();

    layout->addWidget(fileAction->getLoadSelectionAction().createWidget(this));
    layout->addWidget(fileAction->getSaveSelectionAction().createWidget(this));

    setLayout(layout);
}

bool OptionsAction::OptionsStringListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (role == Qt::CheckStateRole && index.isValid()) {
        if (value == Qt::Checked)
            checkedItems.insert(index.row());
        else
            checkedItems.remove(index.row());
        emit dataChanged(index, index);
        return true;
    }
    return QStringListModel::setData(index, value, role);
}

QVariant OptionsAction::OptionsStringListModel::data(const QModelIndex& index, int role) const
{
    if (role == Qt::CheckStateRole && index.isValid())
        return (checkedItems.contains(index.row())) ? Qt::Checked : Qt::Unchecked;

    return QStringListModel::data(index, role);
}

Qt::ItemFlags OptionsAction::OptionsStringListModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = QStringListModel::flags(index) | Qt::ItemIsEditable;

    if (index.isValid())
        flags |= Qt::ItemIsUserCheckable;

    return flags;
}

OptionsAction::OptionsStringListModel::OptionsStringListModel(const QStringList& strings, QObject* parent /*= nullptr*/) :
    QStringListModel(strings, parent)
{
}

}
