#include "StringsAction.h"
#include "Application.h"

#include <QHBoxLayout>

namespace hdps::gui {

StringsAction::StringsAction(QObject* parent, const QString& title /*= ""*/, const QStringList& strings /*= QStringList()*/, const QStringList& defaultStrings /*= QStringList()*/) :
    WidgetAction(parent),
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

void StringsAction::connectToPublicAction(WidgetAction* publicAction)
{
    auto publicStringsAction = dynamic_cast<StringsAction*>(publicAction);

    Q_ASSERT(publicStringsAction != nullptr);

    connect(this, &StringsAction::stringsChanged, publicStringsAction, &StringsAction::setStrings);
    connect(publicStringsAction, &StringsAction::stringsChanged, this, &StringsAction::setStrings);

    setStrings(publicStringsAction->getStrings());

    WidgetAction::connectToPublicAction(publicAction);
}

void StringsAction::disconnectFromPublicAction()
{
    auto publicStringsAction = dynamic_cast<StringsAction*>(getPublicAction());

    Q_ASSERT(publicStringsAction != nullptr);

    disconnect(this, &StringsAction::stringsChanged, publicStringsAction, &StringsAction::setStrings);
    disconnect(publicStringsAction, &StringsAction::stringsChanged, this, &StringsAction::setStrings);

    WidgetAction::disconnectFromPublicAction();
}

WidgetAction* StringsAction::getPublicCopy() const
{
    return new StringsAction(parent(), text(), getStrings(), getDefaultStrings());
}

void StringsAction::fromVariantMap(const QVariantMap& variantMap)
{
    if (!variantMap.contains("value"))
        return;

    setStrings(variantMap["value"].toStringList());
}

QVariantMap StringsAction::toVariantMap() const
{
    return {
        { "value", QVariant::fromValue(_strings) }
    };
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

StringsAction::Widget::Widget(QWidget* parent, StringsAction* stringsAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, stringsAction, widgetFlags),
    _model(),
    _filterModel(),
    _hierarchyWidget(this, stringsAction->getCategory(), _model, &_filterModel, false),
    _nameAction(this, "Name"),
    _addAction(this),
    _removeAction(this)
{
    _hierarchyWidget.getTreeView().setRootIsDecorated(false);
    _hierarchyWidget.setWindowIcon(stringsAction->icon());
    _hierarchyWidget.setHeaderHidden(true);
    
    _filterModel.setFilterKeyColumn(0);

    auto& filterGroupAction = _hierarchyWidget.getFilterGroupAction();

    filterGroupAction.setLabelWidthFixed(40);
    filterGroupAction.setPopupSizeHint(QSize(350, 0));

    _nameAction.setConnectionPermissionsToNone();
    _addAction.setConnectionPermissionsToNone();
    _removeAction.setConnectionPermissionsToNone();

    _nameAction.setClearable(true);
    _nameAction.setPlaceHolderString(QString("Enter %1 name here...").arg(stringsAction->getCategory().toLower()));

    _addAction.setIcon(Application::getIconFont("FontAwesome").getIcon("plus"));
    _addAction.setToolTip(QString("Add entered %1").arg(stringsAction->getCategory().toLower()));

    _removeAction.setIcon(Application::getIconFont("FontAwesome").getIcon("trash"));
    _removeAction.setToolTip(QString("Remove selected %1(s)").arg(stringsAction->getCategory().toLower()));

    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(&_hierarchyWidget, 1);

    auto toolbarLayout = new QHBoxLayout();

    toolbarLayout->addWidget(_nameAction.createWidget(this), 1);
    toolbarLayout->addWidget(_addAction.createWidget(this, TriggerAction::Icon));
    toolbarLayout->addWidget(_removeAction.createWidget(this, TriggerAction::Icon));
    toolbarLayout->addWidget(_hierarchyWidget.getFilterGroupAction().createCollapsedWidget(this));

    layout->addLayout(toolbarLayout);

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
        _model.clear();

        for (const auto& string : strings) {
            auto row = new QStandardItem(stringsAction->icon(), string);

            row->setEditable(false);

            _model.appendRow(row);
        }

        _model.setHeaderData(0, Qt::Horizontal, "Name", Qt::UserRole);
        _model.setHeaderData(0, Qt::Horizontal, "Name", Qt::DisplayRole);

        emit _model.layoutChanged();

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

    connect(&_hierarchyWidget.getSelectionModel(), &QItemSelectionModel::selectionChanged, this, updateActions);
    connect(&_model, &QStandardItemModel::layoutChanged, this, updateActions);

    updateActions();
}

}
