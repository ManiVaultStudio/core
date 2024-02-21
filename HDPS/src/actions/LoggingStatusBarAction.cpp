// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LoggingStatusBarAction.h"

#ifdef _DEBUG
    #define LOGGING_STATUS_BAR_ACTION_VERBOSE
#endif

#include <QGuiApplication>
#include <QClipBoard>

namespace mv::gui {

LoggingStatusBarAction::LoggingStatusBarAction(QObject* parent, const QString& title) :
    HorizontalGroupAction(parent, title),
    _model(),
    _filterModel(this),
    _lastMessageAction(this, "Last message"),
    _recordsAction(this, "Records")
{
    setShowLabels(false);

    addAction(&_lastMessageAction);
    addAction(&_recordsAction, -1, [this](WidgetAction* action, QWidget* widget) -> void {
        auto hierarchyWidget = widget->findChild<HierarchyWidget*>("HierarchyWidget");

        Q_ASSERT(hierarchyWidget != nullptr);

        if (hierarchyWidget == nullptr)
            return;

        hierarchyWidget->setHeaderHidden(true);

        hierarchyWidget->getFilterGroupAction().setVisible(false);
        hierarchyWidget->getFilterColumnAction().setVisible(false);

        auto treeView = widget->findChild<QTreeView*>("TreeView");

        Q_ASSERT(treeView != nullptr);

        if (treeView == nullptr)
            return;

        treeView->setRootIsDecorated(false);

        treeView->setColumnHidden(static_cast<int>(LoggingModel::Column::Number), true);
        treeView->setColumnHidden(static_cast<int>(LoggingModel::Column::Type), true);
        treeView->setColumnHidden(static_cast<int>(LoggingModel::Column::FileAndLine), true);
        treeView->setColumnHidden(static_cast<int>(LoggingModel::Column::Function), true);
        treeView->setColumnHidden(static_cast<int>(LoggingModel::Column::Category), true);

        connect(treeView, &QTreeView::customContextMenuRequested, [this, treeView](const QPoint& point)
        {
            const auto selectedRows = treeView->selectionModel()->selectedRows();

            if (selectedRows.isEmpty())
                return;

            QMenu contextMenu;

            auto* copyAction = contextMenu.addAction(tr("&Copy"), [this, selectedRows] {
                QStringList messageRecordsString;

                for (const auto& selectedRow : selectedRows) {
                    const auto index            = _filterModel.mapToSource(selectedRow);
                    const auto messageRecord    = static_cast<LoggingModel::Item*>(_model.itemFromIndex(index))->getMessageRecord();

                    messageRecordsString << messageRecord.toString();
                }

                QGuiApplication::clipboard()->setText(messageRecordsString.join("\n"));
            });

            copyAction->setIcon(Application::getIconFont("FontAwesome").getIcon("copy"));

            contextMenu.exec(QCursor::pos());
        });
    });

    _filterModel.setSourceModel(&_model);
    _filterModel.setFilterKeyColumn(static_cast<int>(LoggingModel::Column::Message));

    _lastMessageAction.setEnabled(false);

    _recordsAction.setIconByName("scroll");
    _recordsAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _recordsAction.initialize(&_model, &_filterModel, "Log record");
    _recordsAction.setDefaultWidgetFlag(WidgetActionViewWidget::NoGroupBoxInPopupLayout);
    _recordsAction.setPopupSizeHint(QSize(600, 400));

    connect(&_filterModel, &QSortFilterProxyModel::rowsInserted, this, [this](const QModelIndex& parent, int start, int end) -> void {
        _lastMessageAction.setString(_filterModel.index(end, static_cast<int>(LoggingModel::Column::Message), parent).data(Qt::EditRole).toString());

        QCoreApplication::processEvents();
    });
}

}
