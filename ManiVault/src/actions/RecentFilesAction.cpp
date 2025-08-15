// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "RecentFilesAction.h"
#include "Application.h"
#include "CoreInterface.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QMenu>

#ifdef _DEBUG
    #define RECENT_FILES_ACTION_VERBOSE
#endif

using namespace mv::util;

namespace mv::gui {

RecentFilesAction::RecentFilesAction(QObject* parent, const QString& fileType, const QString& shortcutPrefix /*= ""*/, const QIcon& icon /*= QIcon()*/) :
    WidgetAction(parent, "Recent Files"),
    _listModel(this, fileType),
    _editAction(this, "Edit...")
{
    initialize(fileType, shortcutPrefix);
    setIconByName("clock");

    _editAction.setIconByName("gear");
    _editAction.setDefaultWidgetFlags(TriggerAction::WidgetFlag::Icon);

    connect(&_editAction, &TriggerAction::triggered, this, [this]() -> void {
        auto* dialog = new Dialog(this);
        connect(dialog, &Dialog::finished, dialog, &Dialog::deleteLater);
        dialog->open();
    });
}

QMenu* RecentFilesAction::getMenu(QWidget* parent)
{
    auto menu = new QMenu(parent);
    
    menu->setTitle("Recent...");
    menu->setToolTip(QString("Recently opened %1s").arg(_fileType.toLower()));
    menu->setIcon(StyledIcon("clock"));
    menu->setEnabled(_listModel.rowCount() >= 1);

    auto recentFiles = _listModel.getRecentFiles();

    std::ranges::reverse(recentFiles.begin(), recentFiles.end());

    for (const auto& recentFile : recentFiles) {
	    auto openRecentFileAction = new TriggerAction(this, recentFile.getFilePath());
    	openRecentFileAction->setIcon(StyledIcon("clock"));
    	openRecentFileAction->setToolTip(QString("Open %1").arg(recentFile.getFilePath()));

    	connect(openRecentFileAction, &TriggerAction::triggered, this, [this, recentFile]() -> void {
			emit triggered(recentFile.getFilePath());
		});

    	menu->addAction(openRecentFileAction);
    }

    if (!menu->actions().isEmpty()) {
        menu->addSeparator();
        menu->addAction(&_editAction);
    }
    
    return menu;
}

QString RecentFilesAction::getFileType() const
{
    return _fileType;
}

QString RecentFilesAction::getShortcutPrefix() const
{
    return _shortcutPrefix;
}

void RecentFilesAction::addRecentFilePath(const QString& filePath)
{
#ifdef RECENT_FILE_PATHS_ACTION_VERBOSE
    qDebug() << __FUNCTION__ << filePath;
#endif

    _listModel.addRecentFilePath(filePath);
}

void RecentFilesAction::initialize(const QString& fileType, const QString& shortcutPrefix)
{
    _fileType       = fileType;
    _shortcutPrefix = shortcutPrefix;

    _editAction.setIconByName("gear");
    _editAction.setToolTip(QString("Edit recently opened %1s").arg(_fileType.toLower()));

    _listModel.load();
}

TriggerAction& RecentFilesAction::getEditAction()
{
    return _editAction;
}

const RecentFilesListModel& RecentFilesAction::getModel() const
{
    return _listModel;
}

QStringList RecentFilesAction::getRecentFilePaths() const
{
    return _listModel.getRecentFilePaths();
}

RecentFiles RecentFilesAction::getRecentFiles() const
{
    return _listModel.getRecentFiles();
}

RecentFilesListModel& RecentFilesAction::getModel()
{
    return _listModel;
}

void RecentFilesAction::updateRecentFilePaths()
{
    loadFromSettings();
}

RecentFilesAction::Dialog::Dialog(RecentFilesAction* recentFilesAction) :
    _hierarchyWidget(this, QString("Recent %1").arg(recentFilesAction->getFileType()), recentFilesAction->getModel(), &_filterModel),
    _removeAction(this, "Remove"),
    _okAction(this, "Ok")
{
    setModal(true);
    setWindowIcon(recentFilesAction->icon());
    setWindowTitle(QString("Edit recent %1s").arg(recentFilesAction->getFileType().toLower()));

    auto layout = new QVBoxLayout();

    layout->addWidget(&_hierarchyWidget, 1);

    auto bottomLayout = new QHBoxLayout();

    bottomLayout->addWidget(_removeAction.createWidget(this));
    bottomLayout->addStretch(1);
    bottomLayout->addWidget(_okAction.createWidget(this));

    layout->addLayout(bottomLayout);

    setLayout(layout);

    _okAction.setToolTip("Exit the dialog");

    auto& treeView = _hierarchyWidget.getTreeView();

    treeView.setRootIsDecorated(false);
    treeView.setSortingEnabled(true);
    treeView.setTextElideMode(Qt::ElideMiddle);

    _hierarchyWidget.getExpandAllAction().setVisible(false);
    _hierarchyWidget.getCollapseAllAction().setVisible(false);
    _hierarchyWidget.setWindowIcon(StyledIcon("clock"));

    auto treeViewHeader = treeView.header();

    treeViewHeader->setStretchLastSection(false);
    treeViewHeader->setSortIndicator(static_cast<int>(AbstractRecentFilesModel::Column::DateTime), Qt::DescendingOrder);
    treeViewHeader->resizeSection(static_cast<int>(AbstractRecentFilesModel::Column::DateTime), 150);
    treeViewHeader->setSectionResizeMode(static_cast<int>(AbstractRecentFilesModel::Column::FilePath), QHeaderView::Stretch);
    treeViewHeader->setSectionResizeMode(static_cast<int>(AbstractRecentFilesModel::Column::DateTime), QHeaderView::Fixed);

    _removeAction.setEnabled(false);

    const auto updateRemoveReadOnly = [this, recentFilesAction]() -> void {
        const auto numberOfSelectedRows = _hierarchyWidget.getTreeView().selectionModel()->selectedRows().count();

        _removeAction.setText(QString("Remove %1").arg(numberOfSelectedRows >= 1 ? QString::number(numberOfSelectedRows) : ""));
        _removeAction.setToolTip(QString("Remove %1 recent %2%3").arg(numberOfSelectedRows >= 1 ? QString::number(numberOfSelectedRows) : "", recentFilesAction->getFileType().toLower(), numberOfSelectedRows >= 2 ? "s" : ""));
        _removeAction.setEnabled(numberOfSelectedRows >= 1);
    };

    connect(_hierarchyWidget.getTreeView().selectionModel(), &QItemSelectionModel::selectionChanged, this, updateRemoveReadOnly);
    connect(&_filterModel, &QAbstractItemModel::rowsInserted, this, updateRemoveReadOnly);
    connect(&_filterModel, &QAbstractItemModel::rowsRemoved, this, updateRemoveReadOnly);

    connect(&_removeAction, &TriggerAction::triggered, this, [this, recentFilesAction]() -> void {
        const auto selectedRows = _hierarchyWidget.getTreeView().selectionModel()->selectedRows();

        if (selectedRows.isEmpty())
            return;

        QList<QPersistentModelIndex> selectedRowsPersistent;

        for (const auto& selectedRow : selectedRows)
            selectedRowsPersistent << selectedRow;

        for (const auto& selectedRowPersistent : selectedRowsPersistent)
            recentFilesAction->getModel().removeRecentFilePath(selectedRowPersistent.sibling(selectedRowPersistent.row(), static_cast<int>(AbstractRecentFilesModel::Column::FilePath)).data().toString());

        recentFilesAction->getModel().load();
    });

    connect(&_okAction, &TriggerAction::triggered, this, &Dialog::accept);
}

}
