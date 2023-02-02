#include "PresetsAction.h"
#include "Application.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QMenu>
#include <QLocale>
#include <QInputDialog>
#include <QFileDialog>

#ifdef _DEBUG
    #define PRESETS_ACTION_VERBOSE
#endif

namespace hdps::gui {

PresetsAction::PresetsAction(QObject* parent, WidgetAction* sourceAction, const QString& settingsKey /*= ""*/, const QString& presetType /*= ""*/, const QIcon& icon /*= QIcon()*/) :
    WidgetAction(parent),
    _sourceAction(sourceAction),
    _settingsKey(settingsKey),
    _presetType(presetType),
    _icon(icon),
    _model(this),
    _filterModel(this),
    _editAction(this, "Edit...")
{
    _editAction.setIcon(Application::getIconFont("FontAwesome").getIcon("cog"));
    _editAction.setToolTip(QString("Manage %1 presets").arg(_presetType.toLower()));

    connect(&_editAction, &TriggerAction::triggered, this, [this]() -> void {
        Dialog dialog(this);
        dialog.exec();
    });

    updatePresetsFromSettings();
}

QString PresetsAction::getTypeString() const
{
    return "Presets";
}

QString PresetsAction::getSettingsKey() const
{
    return _settingsKey;
}

QString PresetsAction::getPresetType() const
{
    return _presetType;
}

QIcon PresetsAction::getIcon() const
{
    return _icon;
}

QStandardItemModel& PresetsAction::getModel()
{
    return _model;
}

const PresetsAction::FilterModel& PresetsAction::getFilterModel() const
{
    return _filterModel;
}

TriggerAction& PresetsAction::getEditAction()
{
    return _editAction;
}

void PresetsAction::updatePresetsFromSettings()
{
    _model.setRowCount(0);

    auto presetNames = Application::current()->getSetting(_settingsKey, QVariantMap()).toMap().keys();

    
    for (const auto& presetName : presetNames) {
        auto presetItem = new QStandardItem(_icon, presetName);

        //presetItem->setData(dateTime, Qt::EditRole);

        _model.appendRow({ presetItem });
    }
}

QMenu* PresetsAction::getMenu(QWidget* parent /*= nullptr*/)
{
    auto menu = new QMenu("Presets", parent);

    //auto& fontAwesome = Application::getIconFont("FontAwesome");

    //const auto presetIcon = fontAwesome.getIcon("prescription");

    //menu->setIcon(presetIcon);

    //auto savePresetAction = new QAction("Save");

    //savePresetAction->setIcon(fontAwesome.getIcon("save"));

    //connect(savePresetAction, &TriggerAction::triggered, this, [this, &fontAwesome]() -> void {
    //    QInputDialog inputDialog;

    //    inputDialog.setWindowIcon(fontAwesome.getIcon("list-alt"));
    //    inputDialog.setWindowTitle("Choose preset name");
    //    inputDialog.setInputMode(QInputDialog::TextInput);
    //    inputDialog.setLabelText("Preset name:");

    //    if (inputDialog.exec())
    //        savePreset(inputDialog.textValue());
    //    });

    //menu->addAction(savePresetAction);

    //auto saveDefaultPresetAction = new QAction("Save As Default");

    //saveDefaultPresetAction->setIcon(fontAwesome.getIcon("save"));

    //connect(saveDefaultPresetAction, &TriggerAction::triggered, this, &ViewPlugin::saveDefaultPreset);

    //menu->addAction(saveDefaultPresetAction);

    //auto presetNames = getSetting("Presets").toMap().keys();

    //if (!presetNames.isEmpty())
    //    menu->addSeparator();

    //if (presetNames.contains("Default"))
    //    presetNames.removeOne("Default");

    //presetNames.insert(0, "Default");

    //for (const auto& presetName : presetNames) {
    //    auto loadPresetAction = new QAction(presetName);

    //    loadPresetAction->setIcon(presetIcon);

    //    connect(loadPresetAction, &TriggerAction::triggered, this, [this, presetName]() -> void {
    //        loadPreset(presetName);
    //        });

    //    QWidgetAction* pWidgetAction = new QWidgetAction(0); // no parent-owner?
    //    QLabel* pLabelWidget = new QLabel("Test");           // label widget
    //    pWidgetAction->setDefaultWidget(pLabelWidget);       // label is a widget
    //    pWidgetAction->setCheckable(true);
    //    menu->addAction(pWidgetAction);
    //    menu->addAction(loadPresetAction);

    //    if (presetName == "Default")
    //        menu->addSeparator();
    //}

    //menu->addSeparator();

    //auto importPresetAction = new QAction("Import...");
    //auto exportPresetAction = new QAction("Export...");

    //importPresetAction->setIcon(fontAwesome.getIcon("file-import"));
    //exportPresetAction->setIcon(fontAwesome.getIcon("file-export"));

    //menu->addAction(importPresetAction);
    //menu->addAction(exportPresetAction);

    //connect(importPresetAction, &TriggerAction::triggered, this, &ViewPlugin::importPreset);
    //connect(exportPresetAction, &TriggerAction::triggered, this, &ViewPlugin::exportPreset);

    //menu->addSeparator();

    //auto editPresetsAction = new QAction("Edit...");

    //editPresetsAction->setIcon(fontAwesome.getIcon("cog"));

    //menu->addAction(editPresetsAction);

    //connect(editPresetsAction, &TriggerAction::triggered, this, [this, presetNames, &fontAwesome]() -> void {
    //    });

    return menu;
}

void PresetsAction::loadPreset(const QString& name)
{
#ifdef PRESETS_ACTION_VERBOSE
    qDebug() << __FUNCTION__ << name;
#endif

    //if (name.isEmpty())
    //    return;

    //auto presets = getSetting("Presets", QVariantMap()).toMap();

    //if (!presets.contains(name))
    //    return;

    //fromVariantMap(presets[name].toMap());
}

void PresetsAction::savePreset(const QString& name)
{
#ifdef PRESETS_ACTION_VERBOSE
    qDebug() << __FUNCTION__ << name;
#endif

    //if (name.isEmpty())
    //    return;

    //auto presets = getSetting("Presets", QVariantMap()).toMap();

    //presets[name] = toVariantMap();

    //setSetting("Presets", presets);
}

void PresetsAction::importPreset()
{
#ifdef PRESETS_ACTION_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    QFileDialog fileDialog;

    fileDialog.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("file-import"));
    fileDialog.setWindowTitle("Import Preset");
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    fileDialog.setNameFilters({ "HDPS View Preset (*.hvp)" });
    fileDialog.setDefaultSuffix(".hvp");
    fileDialog.setOption(QFileDialog::DontUseNativeDialog, true);

    if (fileDialog.exec() == 0)
        return;

    if (fileDialog.selectedFiles().count() != 1)
        throw std::runtime_error("Only one file may be selected");

    const auto presetFilePath = fileDialog.selectedFiles().first();

    fromJsonFile(presetFilePath);
    savePreset(QFileInfo(presetFilePath).baseName());
}

void PresetsAction::exportPreset()
{
#ifdef PRESETS_ACTION_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    QFileDialog fileDialog;

    fileDialog.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("file-export"));
    fileDialog.setWindowTitle("Export Preset");
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setNameFilters({ "HDPS View Preset (*.hvp)" });
    fileDialog.setDefaultSuffix(".hvp");
    fileDialog.setOption(QFileDialog::DontUseNativeDialog, true);

    if (fileDialog.exec() == 0)
        return;

    if (fileDialog.selectedFiles().count() != 1)
        throw std::runtime_error("Only one file may be selected");

    toJsonFile(fileDialog.selectedFiles().first());
}

void PresetsAction::loadDefaultPreset()
{
    loadPreset("Default");
}

void PresetsAction::saveDefaultPreset()
{
    savePreset("Default");
}

PresetsAction::FilterModel::FilterModel(QObject* parent /*= nullptr*/) :
    QSortFilterProxyModel(parent)
{
    setFilterKeyColumn(static_cast<int>(PresetsAction::Column::Name));
}

bool PresetsAction::FilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
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

bool PresetsAction::FilterModel::lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const
{
    if (lhs.column() == 0)
        return lhs.data().toString() < rhs.data().toString();

    if (lhs.column() == 1)
        return lhs.data(Qt::EditRole).toDateTime() < rhs.data(Qt::EditRole).toDateTime();

    return false;
}

PresetsAction::Dialog::Dialog(PresetsAction* presetsAction) :
    QDialog(),
    _hierarchyWidget(this, QString("%1 Preset").arg(presetsAction->getPresetType()), presetsAction->getModel(), const_cast<PresetsAction::FilterModel*>(&presetsAction->getFilterModel())),
    _removeAction(this, "Remove"),
    _okAction(this, "Ok")
{
    /*
    setModal(true);
    setWindowIcon(presetsAction->getIcon());
    setWindowTitle(QString("Edit %1 presets").arg(presetsAction->getPresetType().toLower()));

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
    */
}

}
