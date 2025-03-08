// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ActionsWidget.h"
#include "CoreInterface.h"

#include "actions/WidgetActionContextMenu.h"

#include <Application.h>

#include <QDebug>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QStyledItemDelegate>
#include <QScrollBar>
#include <QMenu>

using namespace mv::util;

namespace mv::gui
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
                break;
        }
    }
};

ActionsWidget::ActionsWidget(QWidget* parent, AbstractActionsModel& actionsModel, const QString& itemTypeName /*= "Parameter"*/) :
    QWidget(parent),
    _actionsModel(actionsModel),
    _filterModel(this),
    _hierarchyWidget(this, itemTypeName, actionsModel, &_filterModel),
    _requestContextMenu(),
    _contextMenu(nullptr),
    _highlightedActions()
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(&_hierarchyWidget, 1);

    setLayout(layout);

    _hierarchyWidget.setWindowIcon(StyledIcon("play"));

    auto& treeView = _hierarchyWidget.getTreeView();

    //treeView.setRootIsDecorated(false);

    auto treeViewHeader = treeView.header();

    treeViewHeader->setStretchLastSection(false);

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

    treeViewHeader->setSectionResizeMode(static_cast<int>(AbstractActionsModel::Column::Name), QHeaderView::Stretch);
    treeViewHeader->setSectionResizeMode(static_cast<int>(AbstractActionsModel::Column::Location), QHeaderView::Stretch);

    treeView.setMouseTracking(true);
    treeView.setDragEnabled(true);
    treeView.setAcceptDrops(true);
    treeView.setItemDelegate(new ItemDelegate(this));

    connect(&_hierarchyWidget.getSelectionModel(), &QItemSelectionModel::selectionChanged, this, [this](const QItemSelection& selected, const QItemSelection& deselected) -> void {
        highlightActions(_hierarchyWidget.getTreeView().selectionModel()->selectedRows(), true);
    });

    connect(&_filterModel, &QAbstractItemModel::rowsInserted, this, &ActionsWidget::modelChanged);
    connect(&_filterModel, &QAbstractItemModel::rowsRemoved, this, &ActionsWidget::modelChanged);
    connect(&_filterModel, &QAbstractItemModel::layoutChanged, this, &ActionsWidget::modelChanged);
    
    connect(&treeView, &HierarchyWidgetTreeView::columnHiddenChanged, this, &ActionsWidget::modelChanged);
    connect(&treeView, &HierarchyWidgetTreeView::expanded, this, &ActionsWidget::modelChanged);
    connect(&treeView, &HierarchyWidgetTreeView::collapsed, this, &ActionsWidget::modelChanged);

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

    filterGroupAction.addAction(&_filterModel.getScopeFilterAction());
    filterGroupAction.addAction(&_filterModel.getFilterForceHiddenAction());
    filterGroupAction.addAction(&_filterModel.getFilterForceDisabledAction());
    filterGroupAction.addAction(&_filterModel.getFilterMayPublishAction());
    filterGroupAction.addAction(&_filterModel.getFilterMayConnectAction());
    filterGroupAction.addAction(&_filterModel.getFilterMayDisconnectAction());

    filterGroupAction.setPopupSizeHint(QSize(300, 0));

    connect(&treeView, &QTreeView::customContextMenuRequested, [this](const QPoint& point) {
        WidgetActions selectedActions;

        for (const auto& selectedRow : _hierarchyWidget.getTreeView().selectionModel()->selectedRows())
            selectedActions << _actionsModel.getAction(_filterModel.mapToSource(selectedRow));

        if (selectedActions.isEmpty())
            return;

        _contextMenu = new WidgetActionContextMenu(this, selectedActions);

        if (_requestContextMenu)
            _requestContextMenu(_contextMenu, selectedActions);

        if (!_contextMenu->actions().isEmpty())
            _contextMenu->exec(QCursor::pos());
    });

    _hierarchyWidget.getTreeView().installEventFilter(this);
    _hierarchyWidget.getTreeView().verticalScrollBar()->installEventFilter(this);

    modelChanged();
}

bool ActionsWidget::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::FocusIn:
        {
            if (target == &_hierarchyWidget.getTreeView())
                highlightActions(_hierarchyWidget.getTreeView().selectionModel()->selectedRows(), false);

            break;
        }

        case QEvent::Close:
        case QEvent::FocusOut:
        {
            if (target == &_hierarchyWidget.getTreeView() && _contextMenu.isNull())
                highlightActions(_hierarchyWidget.getTreeView().selectionModel()->selectedRows(), false);

            break;
        }

        case QEvent::Show:
        case QEvent::Hide:
        {
            if (target != _hierarchyWidget.getTreeView().verticalScrollBar())
                break;

            modelChanged();

            break;
        }

        default:
            break;
    }

    return QObject::eventFilter(target, event);
}

mv::ActionsFilterModel& ActionsWidget::getFilterModel()
{
    return _filterModel;
}

HierarchyWidget& ActionsWidget::getHierarchyWidget()
{
    return _hierarchyWidget;
}

void ActionsWidget::modelChanged()
{
    if (_hierarchyWidget.getModel().rowCount() <= 0)
        return;

    auto treeViewHeader = _hierarchyWidget.getTreeView().header();

    treeViewHeader->resizeSections(QHeaderView::ResizeMode::ResizeToContents);
}

void ActionsWidget::highlightActions(const QModelIndexList& selectedRows, bool highlight)
{
    PersistentModelIndices persistentModelIndices;

    for (const auto& selectedRow : selectedRows)
        persistentModelIndices << _hierarchyWidget.toSourceModelIndex(selectedRow);

    highlightActions(persistentModelIndices, highlight);
}

void ActionsWidget::highlightActions(const PersistentModelIndices& highlightModelIndices, bool highlight)
{
    for (const auto& highlightedAction : _highlightedActions)
        highlightedAction->setHighlighted(false);

    _highlightedActions.clear();

    for (const auto& highlightModelIndex : highlightModelIndices) {
        auto action = _actionsModel.getAction(highlightModelIndex);

        if (action == nullptr)
            continue;

        _highlightedActions << action;

        action->setHighlighted(highlight);

        if (!action->isPublic())
            continue;

        for (auto connectedAction : action->getConnectedActions()) {
            connectedAction->setHighlighted(highlight);

            _highlightedActions << connectedAction;
        }
    }
}

void ActionsWidget::setRequestContextMenuCallback(RequestContextMenuFN requestContextMenu)
{
    _requestContextMenu = requestContextMenu;
}

}
