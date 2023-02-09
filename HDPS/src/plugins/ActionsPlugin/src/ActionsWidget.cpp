#include "ActionsWidget.h"

#include <CoreInterface.h>
#include <AbstractActionsManager.h>
#include <Application.h>

#include <QDebug>
#include <QHeaderView>

using namespace hdps;

ActionsWidget::ActionsWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _filterModel(this),
    _hierarchyWidget(this, "Shared Parameter", hdps::actions().getActionsModel(), &_filterModel)
{
    _hierarchyWidget.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("cloud"));
    //_hierarchyWidget.getTreeView().setRootIsDecorated(false);

    _filterModel.getScopeFilterAction().setSelectedOptions({ "Public" });

    _hierarchyWidget.getFilterGroupAction() << _filterModel.getTypeFilterAction();
    _hierarchyWidget.getFilterGroupAction() << _filterModel.getScopeFilterAction();

#ifdef _DEBUG
    _filterModel.getScopeFilterAction().setEnabled(true);
#else
    _filterModel.getScopeFilterAction().setEnabled(false);
#endif

    _hierarchyWidget.getFilterGroupAction().setPopupSizeHint(QSize(250, 0));

    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(&_hierarchyWidget);

    setLayout(layout);

    auto& treeView = _hierarchyWidget.getTreeView();

    treeView.setColumnHidden(static_cast<int>(ActionsModel::Column::ID), true);
    treeView.setTextElideMode(Qt::ElideMiddle);

    auto treeViewHeader = treeView.header();

    treeViewHeader->setStretchLastSection(false);

    treeViewHeader->resizeSection(static_cast<int>(ActionsModel::Column::Name), 300);
    treeViewHeader->resizeSection(static_cast<int>(ActionsModel::Column::ID), 100);

    treeViewHeader->setSectionResizeMode(0, QHeaderView::Stretch);
    treeViewHeader->setSectionResizeMode(1, QHeaderView::Fixed);

    connect(&treeView, &QTreeView::customContextMenuRequested, [this](const QPoint& point) {
        const auto selectedRows = _hierarchyWidget.getTreeView().selectionModel()->selectedRows();

        if (selectedRows.isEmpty())
            return;

        auto allPrivate = true;
        auto allPublic  = true;

        for (const auto& selectedRow : selectedRows) {
            if (!selectedRow.parent().isValid())
                allPrivate = false;

            if (selectedRow.parent().isValid())
                allPublic = false;
        }

        if (allPrivate) {
            QMenu contextMenu;

            contextMenu.addAction(Application::getIconFont("FontAwesome").getIcon("unlink"), "Disconnect parameter(s)", [this, selectedRows] {
                QList<gui::WidgetAction*> privateActions;

                for (const auto& selectedRow : selectedRows)
                    privateActions << hdps::actions().getActionsModel().getAction(_filterModel.mapToSource(selectedRow));
                    
                for (auto privateAction : privateActions)
                    hdps::actions().disconnectPrivateActionFromPublicAction(privateAction);
            });

            contextMenu.exec(QCursor::pos());
        }

        if (allPublic) {
            QMenu contextMenu;

            if (!selectedRows.isEmpty()) {
                contextMenu.addAction(Application::getIconFont("FontAwesome").getIcon("unlink"), "Remove shared parameter(s)", [this, selectedRows] {
                    QList<gui::WidgetAction*> actionsToRemove;

                    for (const auto& selectedRow : selectedRows) {
                        auto publicAction = hdps::actions().getActionsModel().getAction(_filterModel.mapToSource(selectedRow));

                        for (auto connectedAction : publicAction->getConnectedActions())
                            hdps::actions().disconnectPrivateActionFromPublicAction(connectedAction);

                        delete publicAction;
                    }
                });
            }

            QList<gui::WidgetAction*> allConnectedActions;

            for (const auto& selectedRow : selectedRows)
                allConnectedActions << hdps::actions().getActionsModel().getAction(_filterModel.mapToSource(selectedRow))->getConnectedActions();

            if (!allConnectedActions.isEmpty()) {
                contextMenu.addAction(Application::getIconFont("FontAwesome").getIcon("unlink"), "Disconnect connected parameter(s)", [this, allConnectedActions] {
                    for (auto connectedAction : allConnectedActions)
                        hdps::actions().disconnectPrivateActionFromPublicAction(connectedAction);
                });
            }
            
            if (!contextMenu.actions().isEmpty())
                contextMenu.exec(QCursor::pos());
        }
    });
}
