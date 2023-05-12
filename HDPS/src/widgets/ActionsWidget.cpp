#include "ActionsWidget.h"
#include "CoreInterface.h"

#include "actions/WidgetActionContextMenu.h"

#include <Application.h>

#include <QDebug>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QStyledItemDelegate>
#include <QScrollBar>

using namespace hdps::util;

namespace hdps::gui
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

        const auto column = static_cast<AbstractActionsModel::Column>(index.column());

        switch (column)
        {
            case AbstractActionsModel::Column::ForceDisabled:
            case AbstractActionsModel::Column::ForceHidden:
            case AbstractActionsModel::Column::MayPublish:
            case AbstractActionsModel::Column::MayConnect:
            case AbstractActionsModel::Column::MayDisconnect:
            {
                if (!index.data(Qt::EditRole).toBool())
                    option->state &= ~QStyle::State_Enabled;

                break;
            }

            default:
            {
                const auto forceDisabled = index.siblingAtColumn(static_cast<int>(AbstractActionsModel::Column::ForceDisabled)).data(Qt::EditRole).toBool();

                if (column != AbstractActionsModel::Column::ForceDisabled && forceDisabled)
                    option->state &= ~QStyle::State_Enabled;

                break;
            }
        }
    }
};

ActionsWidget::ActionsWidget(QWidget* parent, AbstractActionsModel& actionsModel, const QString& itemTypeName /*= "Parameter"*/) :
    QWidget(parent),
    _actionsModel(actionsModel),
    _filterModel(this),
    _hierarchyWidget(this, itemTypeName, actionsModel, &_filterModel),
    _requestContextMenu()
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(&_hierarchyWidget, 1);

    setLayout(layout);

    _hierarchyWidget.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("play"));

    auto& treeView = _hierarchyWidget.getTreeView();

    //treeView.setRootIsDecorated(false);

    auto treeViewHeader = treeView.header();

    treeViewHeader->setStretchLastSection(false);

    const auto toggleColumnSize = 16;

    treeView.setColumnHidden(static_cast<int>(AbstractActionsModel::Column::Location), true);
    treeView.setColumnHidden(static_cast<int>(AbstractActionsModel::Column::ID), true);
    treeView.setColumnHidden(static_cast<int>(AbstractActionsModel::Column::Scope), true);
    treeView.setColumnHidden(static_cast<int>(AbstractActionsModel::Column::ParentActionId), true);
    treeView.setColumnHidden(static_cast<int>(AbstractActionsModel::Column::IsConnected), true);
    treeView.setColumnHidden(static_cast<int>(AbstractActionsModel::Column::NumberOfConnectedActions), true);
    treeView.setColumnHidden(static_cast<int>(AbstractActionsModel::Column::PublicActionID), true);
    treeView.setColumnHidden(static_cast<int>(AbstractActionsModel::Column::SortIndex), true);
    treeView.setColumnHidden(static_cast<int>(AbstractActionsModel::Column::Stretch), true);
    treeView.setColumnHidden(static_cast<int>(AbstractActionsModel::Column::IsRoot), true);
    treeView.setColumnHidden(static_cast<int>(AbstractActionsModel::Column::IsLeaf), true);
    treeView.setColumnHidden(static_cast<int>(AbstractActionsModel::Column::InternalUseOnly), true);

    treeViewHeader->setSectionResizeMode(static_cast<int>(AbstractActionsModel::Column::Name), QHeaderView::Stretch);
    treeViewHeader->setSectionResizeMode(static_cast<int>(AbstractActionsModel::Column::Location), QHeaderView::Stretch);

    treeView.setMouseTracking(true);

    treeView.setItemDelegate(new ItemDelegate(this));

    connect(&_hierarchyWidget.getSelectionModel(), &QItemSelectionModel::selectionChanged, this, [this](const QItemSelection& selected, const QItemSelection& deselected) -> void {
        highlightSelection(deselected, false);
        highlightSelection(selected, true);
    });

    connect(&_filterModel, &QAbstractItemModel::rowsInserted, this, &ActionsWidget::resizeSectionsToContent);
    connect(&_filterModel, &QAbstractItemModel::rowsRemoved, this, &ActionsWidget::resizeSectionsToContent);
    
    connect(&treeView, &HierarchyWidgetTreeView::columnHiddenChanged, this, &ActionsWidget::resizeSectionsToContent);
    connect(&treeView, &HierarchyWidgetTreeView::expanded, this, &ActionsWidget::resizeSectionsToContent);
    connect(&treeView, &HierarchyWidgetTreeView::collapsed, this, &ActionsWidget::resizeSectionsToContent);

    connect(&_hierarchyWidget.getTreeView(), &QTreeView::clicked, this, [this](const QModelIndex& index) -> void {
        static const QVector<int> toggleColumns = {
            static_cast<int>(AbstractActionsModel::Column::ForceDisabled),
            static_cast<int>(AbstractActionsModel::Column::ForceHidden),
            static_cast<int>(AbstractActionsModel::Column::MayPublish),
            static_cast<int>(AbstractActionsModel::Column::MayConnect),
            static_cast<int>(AbstractActionsModel::Column::MayDisconnect)
        };

        if (!toggleColumns.contains(index.column()))
            return;

        const auto sourceModelIndex = _hierarchyWidget.toSourceModelIndex(index);

        auto& actionsModel = const_cast<QAbstractItemModel&>(_hierarchyWidget.getModel());

        actionsModel.setData(sourceModelIndex, !actionsModel.data(sourceModelIndex, Qt::EditRole).toBool(), Qt::EditRole);
    });

    auto& filterGroupAction = _hierarchyWidget.getFilterGroupAction();

    filterGroupAction << _filterModel.getScopeFilterAction();
    filterGroupAction << _filterModel.getHideInternalUseAction();
    filterGroupAction << _filterModel.getFilterEnabledAction();
    filterGroupAction << _filterModel.getFilterVisibilityAction();
    filterGroupAction << _filterModel.getFilterMayPublishAction();
    filterGroupAction << _filterModel.getFilterMayConnectAction();
    filterGroupAction << _filterModel.getFilterMayDisconnectAction();
    filterGroupAction << _filterModel.getRemoveFiltersAction();

    filterGroupAction.setPopupSizeHint(QSize(300, 0));

    connect(&treeView, &QTreeView::customContextMenuRequested, [this](const QPoint& point) {
        WidgetActions selectedActions;

        for (const auto& selectedRow : _hierarchyWidget.getTreeView().selectionModel()->selectedRows())
            selectedActions << _actionsModel.getAction(_filterModel.mapToSource(selectedRow));

        if (selectedActions.isEmpty())
            return;

        auto contextMenu = new WidgetActionContextMenu(this, selectedActions);

        if (_requestContextMenu)
            _requestContextMenu(contextMenu, selectedActions);

        if (!contextMenu->actions().isEmpty())
            contextMenu->exec(QCursor::pos());
    });

    _hierarchyWidget.getTreeView().installEventFilter(this);
    _hierarchyWidget.getTreeView().verticalScrollBar()->installEventFilter(this);

    resizeSectionsToContent();
}

bool ActionsWidget::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Enter:
        {
            if (target != &_hierarchyWidget.getTreeView())
                highlightSelection(_hierarchyWidget.getTreeView().selectionModel()->selection(), true);

            break;
        }

        case QEvent::Leave:
        {
            if (target != &_hierarchyWidget.getTreeView())
                highlightSelection(_hierarchyWidget.getTreeView().selectionModel()->selection(), false);

            break;
        }

        case QEvent::Show:
        case QEvent::Hide:
        {
            if (target != _hierarchyWidget.getTreeView().verticalScrollBar())
                break;

            resizeSectionsToContent();

            break;
        }
            
        default:
            break;
    }

    return QObject::eventFilter(target, event);
}

hdps::ActionsFilterModel& ActionsWidget::getFilterModel()
{
    return _filterModel;
}

HierarchyWidget& ActionsWidget::getHierarchyWidget()
{
    return _hierarchyWidget;
}

void ActionsWidget::resizeSectionsToContent()
{
    if (_hierarchyWidget.getModel().rowCount() <= 0)
        return;

    auto treeViewHeader = _hierarchyWidget.getTreeView().header();

    treeViewHeader->resizeSections(QHeaderView::ResizeMode::ResizeToContents);
}

void ActionsWidget::highlightSelection(const QItemSelection& selection, bool highlight)
{
    for (const auto& range : selection)
        for (const auto& index : range.indexes())
            _actionsModel.getAction(_hierarchyWidget.toSourceModelIndex(index))->setHighlighted(highlight);
}

void ActionsWidget::setRequestContextMenuCallback(RequestContextMenuFN requestContextMenu)
{
    _requestContextMenu = requestContextMenu;
}

}
