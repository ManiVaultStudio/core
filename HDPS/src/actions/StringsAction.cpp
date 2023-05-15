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
    _defaultStrings()
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
    _model(stringsAction->icon(), this),
    _filterModel(),
    _hierarchyWidget(this, stringsAction->getCategory(), _model, &_filterModel, false),
    _nameAction(this, "Name"),
    _addAction(this, "Add"),
    _removeAction(this, "Remove")
{
    resize(0, 150);

    _hierarchyWidget.getTreeView().setRootIsDecorated(false);
    _hierarchyWidget.setWindowIcon(stringsAction->icon());
    _hierarchyWidget.setHeaderHidden(true);
    
    _filterModel.setFilterKeyColumn(0);

    auto& filterGroupAction = _hierarchyWidget.getFilterGroupAction();

    filterGroupAction.setLabelWidthFixed(40);
    filterGroupAction.setPopupSizeHint(QSize(350, 0));

    _nameAction.setClearable(true);
    _nameAction.setPlaceHolderString(QString("Enter %1 name here...").arg(stringsAction->getCategory().toLower()));

    _addAction.setIcon(Application::getIconFont("FontAwesome").getIcon("plus"));
    _addAction.setToolTip(QString("Add entered %1").arg(stringsAction->getCategory().toLower()));

    _removeAction.setIcon(Application::getIconFont("FontAwesome").getIcon("trash"));
    _removeAction.setToolTip(QString("Remove selected %1(s)").arg(stringsAction->getCategory().toLower()));

    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(&_hierarchyWidget, 1);

    if (widgetFlags & StringsAction::WidgetFlag::MayEdit) {
        auto toolbarLayout = new QHBoxLayout();

        toolbarLayout->addWidget(_nameAction.createWidget(this), 1);
        toolbarLayout->addWidget(_addAction.createWidget(this, TriggerAction::Icon));
        toolbarLayout->addWidget(_removeAction.createWidget(this, TriggerAction::Icon));
        toolbarLayout->addWidget(_hierarchyWidget.getFilterGroupAction().createCollapsedWidget(this));

        layout->addLayout(toolbarLayout);
    }

    setLayout(layout);

    const auto updateActions = [this]() -> void {
        const auto selectedRows = _hierarchyWidget.getSelectionModel().selectedRows();

        _nameAction.setEnabled(selectedRows.isEmpty());

        if (selectedRows.count() == 1)
            _nameAction.setString(selectedRows.first().data(Qt::DisplayRole).toString());

        if (selectedRows.count() >= 2) {
            QStringList selected;

            for (const auto& selectedRow : selectedRows)
                selected << selectedRow.data(Qt::DisplayRole).toString();

            _nameAction.setString(selected.join(", "));
        }

        _addAction.setEnabled(selectedRows.isEmpty() && !_nameAction.getString().isEmpty());
        _removeAction.setEnabled(!selectedRows.isEmpty());
    };

    const auto updateModel = [this, stringsAction, updateActions](const QStringList& strings) -> void {
        if (strings != _model.stringList())
            _model.setStringList(strings);

        for (int rowIndex = 0; rowIndex < _model.rowCount(); rowIndex++)
            _model.setData(_model.index(rowIndex), stringsAction->icon(), Qt::DecorationRole);

        _model.setHeaderData(0, Qt::Horizontal, "Name", Qt::UserRole);
        _model.setHeaderData(0, Qt::Horizontal, "Name", Qt::DisplayRole);

        updateActions();
    };

    connect(stringsAction, &StringsAction::stringsChanged, this, updateModel);
    connect(&_nameAction, &StringAction::stringChanged, this, updateActions);

    connect(&_addAction, &TriggerAction::triggered, this, [this, stringsAction]() -> void {
        if (_nameAction.getString().isEmpty())
            return;

        stringsAction->addString(_nameAction.getString());

        _nameAction.setString("");
    });

    connect(&_removeAction, &TriggerAction::triggered, this, [this, stringsAction]() -> void {
        QStringList stringsToRemove;

        for (const auto& selectedRow : _hierarchyWidget.getSelectionModel().selectedRows())
            stringsToRemove << selectedRow.data(Qt::DisplayRole).toString();
        
        stringsAction->removeStrings(stringsToRemove);

        _nameAction.setString("");
    });

    connect(&_hierarchyWidget.getSelectionModel(), &QItemSelectionModel::selectionChanged, this, [this, updateActions](const QItemSelection& selected, const QItemSelection& deselected) -> void {
        if (selected.count() == 0 && deselected.count() >= 1)
            _nameAction.setString("");

        updateActions();
    });

    connect(&_model, &QStandardItemModel::layoutChanged, this, updateActions);

    const auto updateAction = [this, stringsAction]() -> void {
        stringsAction->setStrings(_model.stringList());
    };

    connect(&_model, &QAbstractItemModel::rowsInserted, this, updateAction);
    connect(&_model, &QAbstractItemModel::rowsRemoved, this, updateAction);

    updateModel(stringsAction->getStrings());
    updateActions();

    emit _model.layoutChanged();
}

}
