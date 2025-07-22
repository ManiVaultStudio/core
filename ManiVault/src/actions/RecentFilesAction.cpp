// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "RecentFilesAction.h"
#include "Application.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QMenu>

#ifdef _DEBUG
    #define RECENT_FILES_ACTION_VERBOSE
#endif

using namespace mv::util;

namespace mv::gui {

RecentFilesAction::RecentFilesAction(QObject* parent, const QString& settingsKey /*= ""*/, const QString& fileType /*= ""*/, const QString& shortcutPrefix /*= ""*/, const QIcon& icon /*= QIcon()*/) :
    WidgetAction(parent, "Recent Files"),
    _model(this),
    _editAction(this, "Edit...")
{
    Q_ASSERT(!settingsKey.isEmpty());

    initialize(settingsKey, fileType, shortcutPrefix);

    connect(&_editAction, &TriggerAction::triggered, this, [this]() -> void {
        auto* dialog = new Dialog(this);
        connect(dialog, &Dialog::finished, dialog, &Dialog::deleteLater);
        dialog->open();
    });
}

QMenu* RecentFilesAction::getMenu()
{
    auto menu = new QMenu();

    menu->setTitle("Recent...");
    menu->setToolTip(QString("Recently opened %1s").arg(_fileType.toLower()));
    menu->setIcon(StyledIcon("clock"));
    menu->setEnabled(_model.rowCount() >= 1);

    for (auto action : _model.getActions())
        menu->addAction(action);
    
    if (!_model.getActions().isEmpty()) {
        menu->addSeparator();
        menu->addAction(&_editAction);
    }

    return menu;
}

QString RecentFilesAction::getSettingsKey() const
{
    return _settingsKey;
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

    _model.addRecentFilePath(filePath);
}

void RecentFilesAction::initialize(const QString& settingsKey, const QString& fileType, const QString& shortcutPrefix)
{
    _settingsKey    = settingsKey;
    _fileType       = fileType;
    _shortcutPrefix = shortcutPrefix;

    _editAction.setIconByName("gear");
    _editAction.setToolTip(QString("Edit recently opened %1s").arg(_fileType.toLower()));

    _model.loadFromSettings();
}

TriggerAction& RecentFilesAction::getEditAction()
{
    return _editAction;
}

RecentFilesAction::Model& RecentFilesAction::getModel()
{
    return _model;
}

QStringList RecentFilesAction::getRecentFilePaths() const
{
    return _model.getRecentFilePaths();
}

void RecentFilesAction::updateRecentFilePaths()
{
    loadFromSettings();
}

RecentFiles RecentFilesAction::getRecentFiles() const
{
    RecentFiles recentFiles;

    for (int rowIndex = 0; rowIndex < _model.rowCount(); rowIndex++)
        recentFiles << RecentFile(_model.data(_model.index(rowIndex, static_cast<int>(Model::Column::FilePath))).toString(), _model.data(_model.index(rowIndex, static_cast<int>(Model::Column::DateTime))).toDateTime());

    return recentFiles;
}

QMap<RecentFilesAction::Model::Column, QPair<QString, QString>> RecentFilesAction::Model::columnInfo = QMap<RecentFilesAction::Model::Column, QPair<QString, QString>>({
    { RecentFilesAction::Model::Column::FilePath, { "File path", "Location of the recent file" }},
    { RecentFilesAction::Model::Column::DateTime, { "Date and time", "Date and time when the file was opened" }}
});

RecentFilesAction::Model::Model(RecentFilesAction* recentFilePathsAction) :
    QStandardItemModel(recentFilePathsAction),
    _recentFilesAction(recentFilePathsAction),
    _actions()
{
}

void RecentFilesAction::Model::loadFromSettings()
{
    setRowCount(0);

    for (auto action : _actions)
        delete action;

    _actions.clear();

    const auto recentFilePaths = Application::current()->getSetting(_recentFilesAction->getSettingsKey(), QVariantList()).toList();

    std::int32_t shortcutIndex = 1;

    for (const auto& recentFilePath : recentFilePaths) {
        const auto filePath = recentFilePath.toMap()["FilePath"].toString();
        const auto dateTime = recentFilePath.toMap()["DateTime"].toDateTime();

        if (!QFileInfo(filePath).exists())
            continue;

        auto filePathItem = new QStandardItem(_recentFilesAction->icon(), filePath);
        auto dateTimeItem = new QStandardItem(dateTime.toString("ddd MMMM d yy"));

        dateTimeItem->setData(dateTime, Qt::EditRole);

        appendRow({ filePathItem, dateTimeItem });

        auto recentFilePathAction = new TriggerAction(this, filePath);

        recentFilePathAction->setIcon(_recentFilesAction->icon());

        connect(recentFilePathAction, &TriggerAction::triggered, this, [this, filePath]() -> void {
            emit _recentFilesAction->triggered(filePath);
        });

        _actions << recentFilePathAction;

        shortcutIndex++;
    }

    const auto setHeader = [this](Column column) -> void {
        auto headerItem = new QStandardItem(columnInfo[column].first);

        headerItem->setToolTip(columnInfo[column].second);

        setHorizontalHeaderItem(static_cast<int>(column), headerItem);
    };
    
    setHeader(Column::FilePath);
    setHeader(Column::DateTime);

    emit _recentFilesAction->recentFilesChanged(_recentFilesAction->getRecentFiles());
}

void RecentFilesAction::Model::addRecentFilePath(const QString& filePath)
{
    auto recentFilePaths = Application::current()->getSetting(_recentFilesAction->getSettingsKey(), QVariantList()).toList();

    QVariantMap recentFilePath{
        { "FilePath", filePath },
        { "DateTime", QDateTime::currentDateTime() }
    };

    QVariantList recentFilePathsToKeep;

    for (auto recentFilePath : recentFilePaths)
        if (filePath != recentFilePath.toMap()["FilePath"].toString())
            recentFilePathsToKeep << recentFilePath;

    recentFilePathsToKeep.insert(0, recentFilePath);

    Application::current()->setSetting(_recentFilesAction->getSettingsKey(), recentFilePathsToKeep);

    loadFromSettings();

    emit _recentFilesAction->recentFilesChanged(_recentFilesAction->getRecentFiles());
}

void RecentFilesAction::Model::removeRecentFilePath(const QString& filePath)
{
    auto recentFilePaths = Application::current()->getSetting(_recentFilesAction->getSettingsKey(), QVariantList()).toList();

    QVariantList recentFilePathsToKeep;

    for (auto recentFilePath : recentFilePaths)
        if (filePath != recentFilePath.toMap()["FilePath"].toString())
            recentFilePathsToKeep << recentFilePath;

    Application::current()->setSetting(_recentFilesAction->getSettingsKey(), recentFilePathsToKeep);

    emit _recentFilesAction->recentFilesChanged(_recentFilesAction->getRecentFiles());
}

QList<TriggerAction*> RecentFilesAction::Model::getActions()
{
    return _actions;
}

QStringList RecentFilesAction::Model::getRecentFilePaths() const
{
    QStringList recentFilePaths;

    for (int rowIndex = 0; rowIndex < rowCount(); rowIndex++)
        recentFilePaths << data(index(rowIndex, static_cast<int>(Column::FilePath))).toString();

    return recentFilePaths;
}

RecentFilesAction::Dialog::Dialog(RecentFilesAction* recentFilePathsAction) :
    QDialog(),
    _hierarchyWidget(this, QString("Recent %1").arg(recentFilePathsAction->getFileType()), recentFilePathsAction->getModel(), const_cast<RecentFilesAction::FilterModel*>(&recentFilePathsAction->getFilterModel())),
    _removeAction(this, "Remove"),
    _okAction(this, "Ok")
{
    setModal(true);
    setWindowIcon(recentFilePathsAction->icon());
    setWindowTitle(QString("Edit recent %1s").arg(recentFilePathsAction->getFileType().toLower()));

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

    auto treeViewHeader = treeView.header();

    treeViewHeader->setStretchLastSection(false);

    treeViewHeader->setSortIndicator(static_cast<int>(RecentFilesAction::Model::Column::DateTime), Qt::DescendingOrder);

    treeViewHeader->resizeSection(static_cast<int>(RecentFilesAction::Model::Column::DateTime), 150);

    treeViewHeader->setSectionResizeMode(static_cast<int>(RecentFilesAction::Model::Column::FilePath), QHeaderView::Stretch);
    treeViewHeader->setSectionResizeMode(static_cast<int>(RecentFilesAction::Model::Column::DateTime), QHeaderView::Fixed);

    _removeAction.setEnabled(false);

    const auto updateRemoveReadOnly = [this, recentFilePathsAction]() -> void {
        const auto numberOfSelectedRows = _hierarchyWidget.getTreeView().selectionModel()->selectedRows().count();

        _removeAction.setText(QString("Remove %1").arg(numberOfSelectedRows >= 1 ? QString::number(numberOfSelectedRows) : ""));
        _removeAction.setToolTip(QString("Remove %1 recent %2%3").arg(numberOfSelectedRows >= 1 ? QString::number(numberOfSelectedRows) : "", recentFilePathsAction->getFileType().toLower(), numberOfSelectedRows >= 2 ? "s" : ""));
        _removeAction.setEnabled(numberOfSelectedRows >= 1);
    };

    connect(_hierarchyWidget.getTreeView().selectionModel(), &QItemSelectionModel::selectionChanged, this, updateRemoveReadOnly);
    connect(&recentFilePathsAction->getFilterModel(), &QAbstractItemModel::rowsInserted, this, updateRemoveReadOnly);
    connect(&recentFilePathsAction->getFilterModel(), &QAbstractItemModel::rowsRemoved, this, updateRemoveReadOnly);

    connect(&_removeAction, &TriggerAction::triggered, this, [this, recentFilePathsAction]() -> void {
        const auto selectedRows = _hierarchyWidget.getTreeView().selectionModel()->selectedRows();

        if (selectedRows.isEmpty())
            return;

        for (const auto& selectedRow : selectedRows)
            recentFilePathsAction->getModel().removeRecentFilePath(selectedRow.siblingAtColumn(static_cast<int>(RecentFilesAction::Model::Column::FilePath)).data().toString());

        recentFilePathsAction->getModel().loadFromSettings();
    });

    connect(&_okAction, &TriggerAction::triggered, this, &Dialog::accept);
}

RecentFilesAction::FilterModel::FilterModel(QObject* parent /*= nullptr*/) :
    QSortFilterProxyModel(parent)
{
    setFilterKeyColumn(static_cast<int>(RecentFilesAction::Model::Column::FilePath));
}

bool RecentFilesAction::FilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    const auto index = sourceModel()->index(row, 0, parent);

    if (!index.isValid())
        return true;

    if (filterRegularExpression().isValid()) {
        const auto key = sourceModel()->data(index.siblingAtColumn(filterKeyColumn()), filterRole()).toString();

        if (!key.contains(filterRegularExpression()))
            return false;
    }

    return true;
}

bool RecentFilesAction::FilterModel::lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const
{
    if (lhs.column() == 0)
        return lhs.data().toString() < rhs.data().toString();

    if (lhs.column() == 1)
        return lhs.data(Qt::EditRole).toDateTime() < rhs.data(Qt::EditRole).toDateTime();

    return false;
}

}
