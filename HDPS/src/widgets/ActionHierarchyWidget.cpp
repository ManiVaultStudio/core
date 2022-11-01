#include "ActionHierarchyWidget.h"
#include "ActionHierarchyModelItem.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QHeaderView>

using namespace hdps::util;

namespace hdps
{

namespace gui
{

/**
 * Tree view item delegate class
 * Qt natively does not support disabled items to be selected, this class solves that
 * When an item (dataset) is locked, merely the visual representation is changed and not the item flags (only appears disabled)
 */
class ItemDelegate : public QStyledItemDelegate {
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    explicit ItemDelegate(QObject* parent = nullptr) :
        QStyledItemDelegate(parent)
    {
    }

protected:

    /**
     * Init the style option(s) for the item delegate (we override the options to paint disabled when not visible etc.)
     */
    void initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const override
    {
        QStyledItemDelegate::initStyleOption(option, index);

        switch (index.column())
        {
            case ActionHierarchyModelItem::Column::Name:
                break;

            case ActionHierarchyModelItem::Column::Visible:
            case ActionHierarchyModelItem::Column::MayPublish:
            case ActionHierarchyModelItem::Column::MayConnect:
            case ActionHierarchyModelItem::Column::MayDisconnect:
            {
                if (!index.data(Qt::EditRole).toBool())
                    option->state &= ~QStyle::State_Enabled;
                break;
            }

            default:
                break;
        }
    }
};

ActionHierarchyWidget::ActionHierarchyWidget(QWidget* parent, WidgetAction* rootAction) :
    QWidget(parent),
    _model(this, rootAction),
    _filterModel(this),
    _hierarchyWidget(this, "Action", _model, &_filterModel),
    _lastHoverModelIndex(),
    _filterVisibleAction(this, "Visible", true, true),
    _filterNotVisibleAction(this, "Not visible", true, true),
    _filterMayPublishAction(this, "May publish", true, true),
    _filterMayNotPublishAction(this, "May not publish", true, true),
    _filterMayConnectAction(this, "May connect", true, true),
    _filterMayNotConnectAction(this, "May not connect", true, true),
    _filterMayDisconnectAction(this, "May disconnect", true, true),
    _filterMayNotDisconnectAction(this, "May not disconnect", true, true)
{
    _filterVisibleAction.setConnectionPermissions(WidgetAction::None);
    _filterNotVisibleAction.setConnectionPermissions(WidgetAction::None);
    _filterMayPublishAction.setConnectionPermissions(WidgetAction::None);
    _filterMayNotPublishAction.setConnectionPermissions(WidgetAction::None);
    _filterMayConnectAction.setConnectionPermissions(WidgetAction::None);
    _filterMayNotConnectAction.setConnectionPermissions(WidgetAction::None);
    _filterMayDisconnectAction.setConnectionPermissions(WidgetAction::None);
    _filterMayNotDisconnectAction.setConnectionPermissions(WidgetAction::None);

    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(&_hierarchyWidget, 1);

    setLayout(layout);

    auto header = _hierarchyWidget.getTreeView().header();

    header->setStretchLastSection(false);

    const auto toggleColumnSize = 16;

    header->resizeSection(ActionHierarchyModelItem::Column::Visible, toggleColumnSize);
    header->resizeSection(ActionHierarchyModelItem::Column::MayPublish, toggleColumnSize);
    header->resizeSection(ActionHierarchyModelItem::Column::MayConnect, toggleColumnSize);
    header->resizeSection(ActionHierarchyModelItem::Column::MayDisconnect, toggleColumnSize);

    header->setSectionResizeMode(ActionHierarchyModelItem::Column::Name, QHeaderView::Stretch);
    header->setSectionResizeMode(ActionHierarchyModelItem::Column::Visible, QHeaderView::Fixed);
    header->setSectionResizeMode(ActionHierarchyModelItem::Column::MayPublish, QHeaderView::Fixed);
    header->setSectionResizeMode(ActionHierarchyModelItem::Column::MayConnect, QHeaderView::Fixed);
    header->setSectionResizeMode(ActionHierarchyModelItem::Column::MayDisconnect, QHeaderView::Fixed);

    _hierarchyWidget.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("play"));
    
    auto& treeView = _hierarchyWidget.getTreeView();

    treeView.setMouseTracking(true);
    treeView.setItemDelegate(new ItemDelegate(this));

    connect(&_hierarchyWidget.getSelectionModel(), &QItemSelectionModel::currentChanged, this, [this](const QModelIndex& current, const QModelIndex& previous) -> void {
        setActionHighlighted(_hierarchyWidget.toSourceModelIndex(current.siblingAtColumn(ActionHierarchyModelItem::Column::Name)), true);
        setActionHighlighted(_hierarchyWidget.toSourceModelIndex(previous.siblingAtColumn(ActionHierarchyModelItem::Column::Name)), false);

        _lastHoverModelIndex = _hierarchyWidget.toSourceModelIndex(current.siblingAtColumn(ActionHierarchyModelItem::Column::Name));
    });

    connect(&_hierarchyWidget.getTreeView(), &QTreeView::clicked, this, [this](const QModelIndex& index) -> void {
        if (index.column() == ActionHierarchyModelItem::Column::Name)
            return;

        auto sourceModelIndex = _hierarchyWidget.toSourceModelIndex(index);

        _model.setData(sourceModelIndex, !_model.data(sourceModelIndex, Qt::EditRole).toBool(), Qt::CheckStateRole);
    });
    
    auto& filterGroupAction = _hierarchyWidget.getFilterGroupAction();

    filterGroupAction << _filterVisibleAction;
    filterGroupAction << _filterNotVisibleAction;
    filterGroupAction << _filterMayPublishAction;
    filterGroupAction << _filterMayNotPublishAction;
    filterGroupAction << _filterMayConnectAction;
    filterGroupAction << _filterMayNotConnectAction;
    filterGroupAction << _filterMayDisconnectAction;
    filterGroupAction << _filterMayNotDisconnectAction;
}

ActionHierarchyWidget::~ActionHierarchyWidget()
{
    if (_lastHoverModelIndex.isValid())
        setActionHighlighted(_lastHoverModelIndex, false);
}

void ActionHierarchyWidget::setActionHighlighted(const QModelIndex& index, bool highlighted)
{
    if (!index.isValid())
        return;

    if (index.internalPointer() == nullptr)
        return;

    auto actionHierarchyModelItem = static_cast<ActionHierarchyModelItem*>(index.internalPointer());

    actionHierarchyModelItem->getAction()->setHighlighted(highlighted);
}

}
}
