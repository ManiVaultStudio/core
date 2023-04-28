#include "ActionsWidget.h"
#include "CoreInterface.h"

#include <Application.h>

#include <QDebug>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QStyledItemDelegate>

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

        switch (static_cast<ActionsModel::Column>(index.column()))
        {
            case ActionsModel::Column::Name:
                break;

            case ActionsModel::Column::Visible:
            case ActionsModel::Column::MayPublish:
            case ActionsModel::Column::MayConnect:
            case ActionsModel::Column::MayDisconnect:
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

ActionsWidget::ActionsWidget(QWidget* parent, ActionsModel& actionsModel) :
    QWidget(parent),
    _filterModel(this),
    _hierarchyWidget(this, "Action", actionsModel, &_filterModel),
    _lastHoverModelIndex()
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(&_hierarchyWidget, 1);

    setLayout(layout);

    _hierarchyWidget.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("play"));

    auto& treeView = _hierarchyWidget.getTreeView();

    //treeView.setRootIndex(_filterModel.mapFromSource(hdps::actions().getModel().getActionIndex(action)));

    auto treeViewHeader = treeView.header();

    treeViewHeader->setStretchLastSection(false);

    const auto toggleColumnSize = 16;

    treeView.setColumnHidden(static_cast<int>(ActionsModel::Column::ID), true);
    treeView.setColumnHidden(static_cast<int>(ActionsModel::Column::Scope), true);
    treeView.setColumnHidden(static_cast<int>(ActionsModel::Column::ParentActionId), true);
    treeView.setColumnHidden(static_cast<int>(ActionsModel::Column::IsConnected), true);
    treeView.setColumnHidden(static_cast<int>(ActionsModel::Column::PublicActionID), true);

    treeViewHeader->setSectionResizeMode(static_cast<int>(ActionsModel::Column::Name), QHeaderView::Stretch);
    treeViewHeader->resizeSections(QHeaderView::ResizeMode::ResizeToContents);

    treeView.setMouseTracking(true);
    treeView.setItemDelegate(new ItemDelegate(this));

    connect(&_hierarchyWidget.getTreeView(), &QTreeView::entered, this, [this](const QModelIndex& index) -> void {
        const auto sourceModelIndex = _hierarchyWidget.toSourceModelIndex(index);

        setActionHighlighted(sourceModelIndex, true);

        if (_lastHoverModelIndex.isValid())
            setActionHighlighted(_lastHoverModelIndex, false);

        _lastHoverModelIndex = _hierarchyWidget.toSourceModelIndex(index);
    });

    const auto numberOfRowsChanged = [this]() -> void {
        if (_lastHoverModelIndex.isValid())
            setActionHighlighted(_lastHoverModelIndex, false);

        _lastHoverModelIndex = QModelIndex();
    };
    
    connect(&_filterModel, &QAbstractItemModel::rowsAboutToBeInserted, this, numberOfRowsChanged);
    connect(&_filterModel, &QAbstractItemModel::rowsAboutToBeRemoved, this, numberOfRowsChanged);

    connect(&_hierarchyWidget.getTreeView(), &QTreeView::clicked, this, [this](const QModelIndex& index) -> void {
        static const QVector<int> toggleColumns = {
            static_cast<int>(ActionsModel::Column::Visible),
            static_cast<int>(ActionsModel::Column::MayPublish),
            static_cast<int>(ActionsModel::Column::MayConnect),
            static_cast<int>(ActionsModel::Column::MayDisconnect)
        };

        if (!toggleColumns.contains(index.column()))
            return;

        const auto sourceModelIndex = _hierarchyWidget.toSourceModelIndex(index);

        auto& actionsModel = const_cast<QAbstractItemModel&>(_hierarchyWidget.getModel());

        actionsModel.setData(sourceModelIndex, !actionsModel.data(sourceModelIndex, Qt::EditRole).toBool(), Qt::EditRole);
    });
    
    auto& filterGroupAction = _hierarchyWidget.getFilterGroupAction();

    filterGroupAction << _filterModel.getHideInternalUseAction();
    filterGroupAction << _filterModel.getFilterEnabledAction();
    filterGroupAction << _filterModel.getFilterVisibilityAction();
    filterGroupAction << _filterModel.getFilterMayPublishAction();
    filterGroupAction << _filterModel.getFilterMayConnectAction();
    filterGroupAction << _filterModel.getFilterMayDisconnectAction();
    filterGroupAction << _filterModel.getRemoveFiltersAction();

    filterGroupAction.setPopupSizeHint(QSize(300, 0));
}

void ActionsWidget::setActionHighlighted(const QModelIndex& index, bool highlighted)
{
    return;
    const auto nameIndex = index.siblingAtColumn(static_cast<int>(ActionsModel::Column::Name));

    if (!nameIndex.isValid())
        return;

    if (nameIndex.internalPointer() == nullptr)
        return;

    auto actionHierarchyModelItem = static_cast<ActionsModel::Item*>(nameIndex.internalPointer());

    actionHierarchyModelItem->getAction()->setHighlighted(highlighted);
}

void ActionsWidget::leaveEvent(QEvent* event)
{
    setActionHighlighted(_lastHoverModelIndex, false);

    _lastHoverModelIndex = QModelIndex();
}

}



















//
//
//
//#include "ActionsWidget.h"
//
//#include <CoreInterface.h>
//#include <AbstractActionsManager.h>
//#include <Application.h>
//
//#include <models/ActionsModel.h>
//
//#include <QDebug>
//#include <QHeaderView>
//
//using namespace hdps;
//
//ActionsWidget::ActionsWidget(QWidget* parent /*= nullptr*/) :
//    QWidget(parent),
//    _filterModel(this),
//    _hierarchyWidget(this, "Shared Parameter", hdps::actions().getActionsModel(), &_filterModel)
//{
//    _hierarchyWidget.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("cloud"));
//    //_hierarchyWidget.getTreeView().setRootIsDecorated(false);
//
//    _filterModel.getScopeFilterAction().setSelectedOptions({ "Public" });
//
//    _hierarchyWidget.getFilterGroupAction() << _filterModel.getTypeFilterAction();
//    _hierarchyWidget.getFilterGroupAction() << _filterModel.getScopeFilterAction();
//    _hierarchyWidget.getFilterGroupAction() << _filterModel.getConnectedFilterAction();
//
//#ifdef _DEBUG
//    _filterModel.getScopeFilterAction().setEnabled(true);
//#else
//    _filterModel.getScopeFilterAction().setEnabled(false);
//#endif
//
//    _hierarchyWidget.getFilterGroupAction().setPopupSizeHint(QSize(250, 0));
//
//    auto layout = new QVBoxLayout();
//
//    layout->setContentsMargins(0, 0, 0, 0);
//
//    layout->addWidget(&_hierarchyWidget);
//
//    setLayout(layout);
//
//    auto& treeView = _hierarchyWidget.getTreeView();
//
//    treeView.setColumnHidden(static_cast<int>(ActionsModel::Column::ID), true);
//    treeView.setTextElideMode(Qt::ElideMiddle);
//
//    auto treeViewHeader = treeView.header();
//
//    treeViewHeader->setStretchLastSection(false);
//
//    treeViewHeader->resizeSection(static_cast<int>(ActionsModel::Column::Name), 300);
//    treeViewHeader->resizeSection(static_cast<int>(ActionsModel::Column::ID), 100);
//
//    treeViewHeader->setSectionResizeMode(0, QHeaderView::Stretch);
//    treeViewHeader->setSectionResizeMode(1, QHeaderView::Fixed);
//
//    connect(&treeView, &QTreeView::customContextMenuRequested, [this](const QPoint& point) {
//        const auto selectedRows = _hierarchyWidget.getTreeView().selectionModel()->selectedRows();
//
//        if (selectedRows.isEmpty())
//            return;
//
//        auto allPrivate = true;
//        auto allPublic = true;
//
//        for (const auto& selectedRow : selectedRows) {
//            if (!selectedRow.parent().isValid())
//                allPrivate = false;
//
//            if (selectedRow.parent().isValid())
//                allPublic = false;
//        }
//
//        if (allPrivate) {
//            QMenu contextMenu;
//
//            contextMenu.addAction(Application::getIconFont("FontAwesome").getIcon("unlink"), "Disconnect parameter(s)", [this, selectedRows] {
//                QList<gui::WidgetAction*> privateActions;
//
//                for (const auto& selectedRow : selectedRows)
//                    privateActions << hdps::actions().getActionsModel().getAction(_filterModel.mapToSource(selectedRow));
//
//                for (auto privateAction : privateActions)
//                    hdps::actions().disconnectPrivateActionFromPublicAction(privateAction);
//                });
//
//            contextMenu.exec(QCursor::pos());
//        }
//
//        if (allPublic) {
//            QMenu contextMenu;
//
//            if (!selectedRows.isEmpty()) {
//                contextMenu.addAction(Application::getIconFont("FontAwesome").getIcon("unlink"), "Remove shared parameter(s)", [this, selectedRows] {
//                    QList<gui::WidgetAction*> actionsToRemove;
//
//                    for (const auto& selectedRow : selectedRows) {
//                        auto publicAction = hdps::actions().getActionsModel().getAction(_filterModel.mapToSource(selectedRow));
//
//                        for (auto connectedAction : publicAction->getConnectedActions())
//                            hdps::actions().disconnectPrivateActionFromPublicAction(connectedAction);
//
//                        delete publicAction;
//                    }
//                    });
//            }
//
//            QList<gui::WidgetAction*> allConnectedActions;
//
//            for (const auto& selectedRow : selectedRows)
//                allConnectedActions << hdps::actions().getActionsModel().getAction(_filterModel.mapToSource(selectedRow))->getConnectedActions();
//
//            if (!allConnectedActions.isEmpty()) {
//                contextMenu.addAction(Application::getIconFont("FontAwesome").getIcon("unlink"), "Disconnect connected parameter(s)", [this, allConnectedActions] {
//                    for (auto connectedAction : allConnectedActions)
//                        hdps::actions().disconnectPrivateActionFromPublicAction(connectedAction);
//                    });
//            }
//
//            if (!contextMenu.actions().isEmpty())
//                contextMenu.exec(QCursor::pos());
//        }
//        });
//}