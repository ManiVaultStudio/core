// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LoggingStatusBarAction.h"

#include "CoreInterface.h"

#ifdef _DEBUG
    #define LOGGING_STATUS_BAR_ACTION_VERBOSE
#endif

#include <QGuiApplication>
#include <QClipboard>
#include <QHeaderView>

using namespace mv;
using namespace mv::gui;

LoggingStatusBarAction::LoggingStatusBarAction(QObject* parent, const QString& title) :
    StatusBarAction(parent, title, "scroll"),
    _model(),
    _filterModel(this),
    _lastMessageAction(this, "Last message"),
    _recordsAction(this, "Records"),
    _clearRecordsAction(this, "Clear"),
    _loadPluginAction(this, "Plugin")
{
    setToolTip("Inspect log messages");

    getBarGroupAction().addAction(&_lastMessageAction);

    _lastMessageAction.setEnabled(false);
    _lastMessageAction.setDefaultWidgetFlags(StringAction::Label);
    _lastMessageAction.setStretch(1);
    _lastMessageAction.setToolTip(toolTip());

    setPopupAction(&_recordsAction);

    _recordsAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _recordsAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::NoGroupBoxInPopupLayout);
    _recordsAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ToolButtonAutoRaise);
    _recordsAction.setPopupSizeHint(QSize(800, 300));
    _recordsAction.initialize(&_model, &_filterModel, "Log record");

    auto checkLoggingPluginAvailable = []() -> bool {
        auto loggingPlugin = mv::plugins().getPluginFactory("Logging");

        if (loggingPlugin)
            return loggingPlugin->getNumberOfInstances() == 0;

        return false;
    };

    _recordsAction.setWidgetConfigurationFunction([this, checkLoggingPluginAvailable](WidgetAction* action, QWidget* widget) -> void {
        _loadPluginAction.setEnabled(checkLoggingPluginAvailable());
        _clearRecordsAction.setEnabled(_filterModel.rowCount() > 0);

        auto hierarchyWidget = widget->findChild<HierarchyWidget*>("HierarchyWidget");

        Q_ASSERT(hierarchyWidget != nullptr);

        if (hierarchyWidget == nullptr)
            return;

        hierarchyWidget->setWindowIcon(StyledIcon("scroll"));

        auto& toolbarAction = hierarchyWidget->getToolbarAction();

        toolbarAction.addAction(&_clearRecordsAction);
        toolbarAction.addAction(&_loadPluginAction);

        auto treeView = widget->findChild<QTreeView*>("TreeView");

        Q_ASSERT(treeView != nullptr);

        if (treeView == nullptr)
            return;

        treeView->setRootIsDecorated(false);
        treeView->setAlternatingRowColors(true);

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

            copyAction->setIcon(StyledIcon("copy"));

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
    _filterModel.setFilterColumn(static_cast<int>(LoggingModel::Column::Message));

    _clearRecordsAction.setEnabled(_filterModel.rowCount() > 0);
    _clearRecordsAction.setIconByName("trash");
    _clearRecordsAction.setDefaultWidgetFlags(TriggerAction::WidgetFlag::Icon);
    _clearRecordsAction.setToolTip("Clear all records");

    _loadPluginAction.setEnabled(checkLoggingPluginAvailable());
    _loadPluginAction.setIconByName("window-maximize");
    _loadPluginAction.setDefaultWidgetFlags(TriggerAction::WidgetFlag::Icon);
    _loadPluginAction.setToolTip("Load logging plugin");

    connect(&_filterModel, &QSortFilterProxyModel::rowsInserted, this, [this](const QModelIndex& parent, int start, int end) -> void {
        _lastMessageAction.setString(_filterModel.index(end, static_cast<int>(LoggingModel::Column::Message), parent).data(Qt::EditRole).toString());
    });

    connect(&_clearRecordsAction, &TriggerAction::triggered, this, [this]() -> void {
        _model.setRowCount(0);
        _lastMessageAction.setString("");
    });

    connect(&_loadPluginAction, &TriggerAction::triggered, this, [this]() -> void {
        mv::plugins().requestViewPlugin("Logging", nullptr, DockAreaFlag::Bottom);

        _loadPluginAction.setEnabled(false);
    });
}
