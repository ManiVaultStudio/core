#include "StringsAction.h"
#include "Application.h"

#include <QHBoxLayout>
#include <QStandardItemModel>

using namespace hdps::util;

namespace hdps::gui {

StringsAction::StringsAction(QObject* parent, const QString& title /*= ""*/, const QStringList& strings /*= QStringList()*/, const QStringList& defaultStrings /*= QStringList()*/) :
    WidgetAction(parent, title),
    _category("String"),
    _strings(),
    _defaultStrings(),
    _toolbarAction(this, "Toolbar"),
    _nameAction(&_toolbarAction, "Name"),
    _addAction(&_toolbarAction, "Add"),
    _removeAction(&_toolbarAction, "Remove")
{
    setText(title);
    setDefaultWidgetFlags(WidgetFlag::Default);
    initialize(strings, defaultStrings);
}

void StringsAction::initialize(const QStringList& strings /*= QStringList()*/, const QStringList& defaultStrings /*= QStringList()*/)
{
    setStrings(strings);
    setDefaultStrings(defaultStrings);
}

QStringList StringsAction::getStrings() const
{
    return _strings;
}

void StringsAction::setStrings(const QStringList& strings)
{
    if (strings == _strings)
        return;

    _strings = strings;

    emit stringsChanged(_strings);

    saveToSettings();
}

QStringList StringsAction::getDefaultStrings() const
{
    return _defaultStrings;
}

void StringsAction::setDefaultStrings(const QStringList& defaultStrings)
{
    if (defaultStrings == _defaultStrings)
        return;

    _defaultStrings = defaultStrings;

    emit defaultStringsChanged(_defaultStrings);
}

bool StringsAction::isResettable()
{
    return _strings != _defaultStrings;
}

void StringsAction::reset()
{
    setStrings(_defaultStrings);
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
    _strings << string;

    emit stringsChanged(_strings);
}

void StringsAction::removeString(const QString& string)
{
    _strings.removeOne(string);

    emit stringsChanged(_strings);
}

void StringsAction::removeStrings(const QStringList& strings)
{
    for (const auto& string : strings)
        _strings.removeOne(string);

    emit stringsChanged(_strings);
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
    _model(this),
    _filterModel(),
    _hierarchyWidget(this, stringsAction->getCategory(), _model, &_filterModel, false)
{
    resize(0, 150);

    _hierarchyWidget.getTreeView().setRootIsDecorated(false);
    _hierarchyWidget.setWindowIcon(stringsAction->icon());
    _hierarchyWidget.setHeaderHidden(true);
    
    _filterModel.setFilterKeyColumn(0);

    auto& filterGroupAction = _hierarchyWidget.getFilterGroupAction();

    filterGroupAction.setLabelWidthFixed(40);
    filterGroupAction.setPopupSizeHint(QSize(350, 0));

    stringsAction->getNameAction().setClearable(true);
    stringsAction->getNameAction().setPlaceHolderString(QString("Enter %1 name here...").arg(stringsAction->getCategory().toLower()));

    stringsAction->getAddAction().setIcon(Application::getIconFont("FontAwesome").getIcon("plus"));
    stringsAction->getAddAction().setToolTip(QString("Add entered %1").arg(stringsAction->getCategory().toLower()));
    stringsAction->getAddAction().setDefaultWidgetFlags(TriggerAction::Icon);

    stringsAction->getRemoveAction().setIcon(Application::getIconFont("FontAwesome").getIcon("trash"));
    stringsAction->getRemoveAction().setToolTip(QString("Remove selected %1(s)").arg(stringsAction->getCategory().toLower()));
    stringsAction->getRemoveAction().setDefaultWidgetFlags(TriggerAction::Icon);

    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(&_hierarchyWidget, 1);

    stringsAction->getToolbarAction().setShowLabels(false);

    stringsAction->getToolbarAction().addAction(&stringsAction->getNameAction());
    stringsAction->getToolbarAction().addAction(&stringsAction->getAddAction());
    stringsAction->getToolbarAction().addAction(&stringsAction->getRemoveAction());

    if (widgetFlags & StringsAction::WidgetFlag::MayEdit)
        layout->addWidget(stringsAction->getToolbarAction().createWidget(this));

    setLayout(layout);

    const auto updateActions = [this, stringsAction]() -> void {
        const auto selectedRows = _hierarchyWidget.getSelectionModel().selectedRows();

        stringsAction->getNameAction().setEnabled(selectedRows.count() <= 1);

        if (selectedRows.count() == 1)
            stringsAction->getNameAction().setString(selectedRows.first().data(Qt::DisplayRole).toString());

        if (selectedRows.count() >= 2) {
            QStringList selected;

            for (const auto& selectedRow : selectedRows)
                selected << selectedRow.data(Qt::DisplayRole).toString();

            stringsAction->getNameAction().setString(selected.join(", "));
        }

        stringsAction->getAddAction().setEnabled(selectedRows.isEmpty() && !stringsAction->getNameAction().getString().isEmpty());
        stringsAction->getRemoveAction().setEnabled(!selectedRows.isEmpty());
    };

    const auto updateModel = [this, stringsAction, updateActions](const QStringList& strings) -> void {
        _model.removeRows(0, _model.rowCount());

        for (const auto& string : strings)
            _model.appendRow(new QStandardItem(stringsAction->icon(), string));

        updateActions();
    };

    connect(stringsAction, &StringsAction::stringsChanged, this, updateModel);
    connect(&stringsAction->getNameAction(), &StringAction::stringChanged, this, updateActions);

    connect(&stringsAction->getAddAction(), &TriggerAction::triggered, this, [this, stringsAction]() -> void {
        if (stringsAction->getNameAction().getString().isEmpty())
            return;

        stringsAction->addString(stringsAction->getNameAction().getString());

        stringsAction->getNameAction().setString("");
    });

    connect(&stringsAction->getRemoveAction(), &TriggerAction::triggered, this, [this, stringsAction]() -> void {
        QStringList stringsToRemove;

        for (const auto& selectedRow : _hierarchyWidget.getSelectionModel().selectedRows())
            stringsToRemove << selectedRow.data(Qt::DisplayRole).toString();
        
        stringsAction->removeStrings(stringsToRemove);

        stringsAction->getNameAction().setString("");
    });

    connect(&_hierarchyWidget.getSelectionModel(), &QItemSelectionModel::selectionChanged, this, [this, updateActions, stringsAction](const QItemSelection& selected, const QItemSelection& deselected) -> void {
        if (selected.count() == 0 && deselected.count() >= 1)
            stringsAction->getNameAction().setString("");

        updateActions();
    });

    connect(&_model, &QStandardItemModel::layoutChanged, this, updateActions);

    const auto updateAction = [this, stringsAction]() -> void {
        stringsAction->setStrings(stringsAction->getStrings());
    };

    connect(&_model, &QAbstractItemModel::rowsInserted, this, updateAction);
    connect(&_model, &QAbstractItemModel::rowsRemoved, this, updateAction);

    updateModel(stringsAction->getStrings());
    updateActions();

    emit _model.layoutChanged();
}

}
