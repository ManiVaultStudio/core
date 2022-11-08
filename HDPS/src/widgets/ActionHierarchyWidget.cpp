#include "ActionHierarchyWidget.h"

#include <models/ActionHierarchyModelItem.h>

#include <Application.h>

#include <QDebug>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QStyledItemDelegate>

using namespace hdps::util;

namespace hdps
{

namespace gui
{

/** Tree view item delegate class for overriding painting of toggle columns */
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

    /** Init the style option(s) for the item delegate (we override the options to paint disabled when not visible etc.) */
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
    _lastHoverModelIndex()
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(&_hierarchyWidget, 1);

    setLayout(layout);

    _hierarchyWidget.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("play"));

    auto& treeView = _hierarchyWidget.getTreeView();

    auto treeViewHeader = treeView.header();

    treeViewHeader->setStretchLastSection(false);

    const auto toggleColumnSize = 16;

    treeViewHeader->resizeSection(ActionHierarchyModelItem::Column::Visible, toggleColumnSize);
    treeViewHeader->resizeSection(ActionHierarchyModelItem::Column::MayPublish, toggleColumnSize);
    treeViewHeader->resizeSection(ActionHierarchyModelItem::Column::MayConnect, toggleColumnSize);
    treeViewHeader->resizeSection(ActionHierarchyModelItem::Column::MayDisconnect, toggleColumnSize);

    treeViewHeader->setSectionResizeMode(ActionHierarchyModelItem::Column::Name, QHeaderView::Stretch);
    treeViewHeader->setSectionResizeMode(ActionHierarchyModelItem::Column::Visible, QHeaderView::Fixed);
    treeViewHeader->setSectionResizeMode(ActionHierarchyModelItem::Column::MayPublish, QHeaderView::Fixed);
    treeViewHeader->setSectionResizeMode(ActionHierarchyModelItem::Column::MayConnect, QHeaderView::Fixed);
    treeViewHeader->setSectionResizeMode(ActionHierarchyModelItem::Column::MayDisconnect, QHeaderView::Fixed);

    treeView.setMouseTracking(true);
    treeView.setItemDelegate(new ItemDelegate(this));

    connect(&_hierarchyWidget.getTreeView(), &QTreeView::entered, this, [this](const QModelIndex& index) -> void {
        return;
        setActionHighlighted(_hierarchyWidget.toSourceModelIndex(index.siblingAtColumn(ActionHierarchyModelItem::Column::Name)), true);

        if (_lastHoverModelIndex.isValid())
            setActionHighlighted(_hierarchyWidget.toSourceModelIndex(_lastHoverModelIndex.siblingAtColumn(ActionHierarchyModelItem::Column::Name)), false);

        _lastHoverModelIndex = index;
    });

    const auto numberOfRowsChanged = [this]() -> void {
        if (_lastHoverModelIndex.isValid())
            setActionHighlighted(_hierarchyWidget.toSourceModelIndex(_lastHoverModelIndex.siblingAtColumn(ActionHierarchyModelItem::Column::Name)), false);

        _lastHoverModelIndex = QModelIndex();
    };
    
    connect(&_filterModel, &QAbstractItemModel::rowsAboutToBeInserted, this, numberOfRowsChanged);
    connect(&_filterModel, &QAbstractItemModel::rowsAboutToBeRemoved, this, numberOfRowsChanged);

    connect(&_hierarchyWidget.getTreeView(), &QTreeView::clicked, this, [this](const QModelIndex& index) -> void {
        if (index.column() == ActionHierarchyModelItem::Column::Name)
            return;

        auto sourceModelIndex = _hierarchyWidget.toSourceModelIndex(index);

        _model.setData(sourceModelIndex, !_model.data(sourceModelIndex, Qt::EditRole).toBool(), Qt::CheckStateRole);
    });
    
    auto& filterGroupAction = _hierarchyWidget.getFilterGroupAction();

    filterGroupAction << _filterModel.getFilterEnabledAction();
    filterGroupAction << _filterModel.getFilterVisibilityAction();
    filterGroupAction << _filterModel.getFilterMayPublishAction();
    filterGroupAction << _filterModel.getFilterMayConnectAction();
    filterGroupAction << _filterModel.getFilterMayDisconnectAction();
    filterGroupAction << _filterModel.getRemoveFiltersAction();

    filterGroupAction.setPopupSizeHint(QSize(300, 0));

    //connect(this, QWidget::clo)
}

ActionHierarchyWidget::~ActionHierarchyWidget()
{
    //const auto lastSourceHoverModelIndex = _lastHoverModelIndex.siblingAtColumn(ActionHierarchyModelItem::Column::Name);

    //if (!lastSourceHoverModelIndex.isValid())
    //    return;

    //auto actionHierarchyModelItem = static_cast<ActionHierarchyModelItem*>(lastSourceHoverModelIndex.internalPointer());

    //actionHierarchyModelItem->getAction()->setHighlighted(false);
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
