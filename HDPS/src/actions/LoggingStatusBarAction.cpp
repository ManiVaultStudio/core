// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LoggingStatusBarAction.h"

#ifdef _DEBUG
    #define LOGGING_STATUS_BAR_ACTION_VERBOSE
#endif

#include <QGuiApplication>
#include <QClipBoard>
#include <QHeaderView>

namespace mv::gui {

LoggingStatusBarAction::LoggingStatusBarAction(QObject* parent, const QString& title) :
    HorizontalGroupAction(parent, title),
    _model(),
    _filterModel(this),
    _lastMessageAction(this, "Last message"),
    _recordsAction(this, "Records"),
    _clearRecordsAction(this, "Clear"),
    _loadPluginAction(this, "Plugin")
{
    setShowLabels(false);

    addAction(&_lastMessageAction);
    addAction(&_recordsAction, -1, [this](WidgetAction* action, QWidget* widget) -> void {
        _loadPluginAction.setEnabled(mv::plugins().getPluginFactory("Logging")->getNumberOfInstances() == 0);
        _clearRecordsAction.setEnabled(_filterModel.rowCount() > 0);

        auto toolButton = widget->findChild<QToolButton*>("ToolButton");

        Q_ASSERT(toolButton != nullptr);

        if (toolButton) {
            toolButton->setAutoRaise(true);
        }

        auto hierarchyWidget = widget->findChild<HierarchyWidget*>("HierarchyWidget");

        Q_ASSERT(hierarchyWidget != nullptr);

        if (hierarchyWidget == nullptr)
            return;

        hierarchyWidget->setWindowIcon(Application::getIconFont("FontAwesome").getIcon("scroll"));

        auto& toolbarAction = hierarchyWidget->getToolbarAction();

        toolbarAction.addAction(&_clearRecordsAction);
        toolbarAction.addAction(&_loadPluginAction);

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

        connect(treeView, &QTreeView::customContextMenuRequested, treeView, [this, treeView](const QPoint& point)
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

        auto treeViewHeader = treeView->header();

        treeViewHeader->setStretchLastSection(true);

        for (int columnIndex = 0; columnIndex < _model.columnCount(); ++columnIndex)
            treeViewHeader->setSectionResizeMode(columnIndex, QHeaderView::ResizeToContents);

        treeViewHeader->resizeSections(QHeaderView::ResizeMode::ResizeToContents);

        connect(treeViewHeader, &QHeaderView::sectionResized, treeView, [treeViewHeader](int logicalIndex, int oldSize, int newSize) -> void {
            treeViewHeader->resizeSections(QHeaderView::ResizeMode::ResizeToContents);
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

    auto& badge = _recordsAction.getBadge();

    badge.setScale(0.5f);

    const auto updateBadgeNumber = [this, &badge]() -> void {
        const auto numberOfRecords = _filterModel.rowCount();

        badge.setEnabled(numberOfRecords > 0);
        badge.setNumber(numberOfRecords);
    };

    updateBadgeNumber();

    _clearRecordsAction.setEnabled(_filterModel.rowCount() > 0);
    _clearRecordsAction.setIconByName("trash");
    _clearRecordsAction.setDefaultWidgetFlags(TriggerAction::WidgetFlag::Icon);
    _clearRecordsAction.setToolTip("Clear all records");

    _loadPluginAction.setEnabled(mv::plugins().getPluginFactory("Logging")->getNumberOfInstances() == 0);
    _loadPluginAction.setIconByName("plug");
    _loadPluginAction.setDefaultWidgetFlags(TriggerAction::WidgetFlag::Icon);
    _loadPluginAction.setToolTip("Load logging plugin");

    connect(&_filterModel, &QSortFilterProxyModel::rowsInserted, this, [this](const QModelIndex& parent, int start, int end) -> void {
        _lastMessageAction.setString(_filterModel.index(end, static_cast<int>(LoggingModel::Column::Message), parent).data(Qt::EditRole).toString());
    });

    connect(&_filterModel, &QSortFilterProxyModel::rowsInserted, this, updateBadgeNumber);
    connect(&_filterModel, &QSortFilterProxyModel::rowsRemoved, this, updateBadgeNumber);
    connect(&_filterModel, &QSortFilterProxyModel::layoutChanged, this, updateBadgeNumber);

    connect(&_clearRecordsAction, &TriggerAction::triggered, this, [this]() -> void {
        _model.setRowCount(0);
    });

    connect(&_loadPluginAction, &TriggerAction::triggered, this, [this]() -> void {
        mv::plugins().requestViewPlugin("Logging", nullptr, DockAreaFlag::Bottom);

        _loadPluginAction.setEnabled(false);
    });
}

}
