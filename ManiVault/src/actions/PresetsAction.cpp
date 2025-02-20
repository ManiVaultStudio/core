// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PresetsAction.h"
#include "Application.h"

#include "actions/DatasetPickerAction.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QMenu>
#include <QFileDialog>
#include <QStringListModel>

#ifdef _DEBUG
    //#define PRESETS_ACTION_VERBOSE
#endif

using namespace mv::util;

namespace mv::gui {

QMap<PresetsAction::Column, QPair<QString, QString>> PresetsAction::columnInfo = QMap<Column, QPair<QString, QString>>({
    { Column::Name, { "Name", "Preset name" }},
    { Column::DateTime, { "Date and time", "Date and time when the preset was changed" }}
});

PresetsAction::PresetsAction(QObject* parent, WidgetAction* sourceAction, const QString& settingsKey /*= ""*/, const QString& presetType /*= ""*/, const util::StyledIcon& icon /*= util::StyledIcon()*/) :
    WidgetAction(parent, "Presets"),
    _sourceAction(sourceAction),
    _settingsKey(settingsKey),
    _presetType(presetType),
    _icon(icon),
    _model(this),
    _filterModel(this),
    _editAction(this, "Edit...")
{
    Q_ASSERT(_sourceAction != nullptr);

    setText("Presets");
    setConnectionPermissionsToForceNone(true);

    _editAction.setIconByName("cog");
    _editAction.setToolTip(QString("Manage %1 presets").arg(_presetType.toLower()));

    connect(&_editAction, &TriggerAction::triggered, this, [this]() -> void {
        auto* dialog = new ManagePresetsDialog(this);
        connect(dialog, &ManagePresetsDialog::finished, dialog, &ManagePresetsDialog::deleteLater);
        dialog->open();
    });

    loadPresetsFromApplicationSettings();
}

QString PresetsAction::getSettingsKey() const
{
    return _settingsKey;
}

QString PresetsAction::getPresetType() const
{
    return _presetType;
}

const util::StyledIcon& PresetsAction::getIcon() const
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

void PresetsAction::loadPresetsFromApplicationSettings()
{
#ifdef PRESETS_ACTION_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    _presets = Application::current()->getSetting(_settingsKey, QVariantMap()).toMap();

    updateModel();
}

void PresetsAction::savePresetsToApplicationSettings() const
{
#ifdef PRESETS_ACTION_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    Application::current()->setSetting(_settingsKey, _presets);
}

QMenu* PresetsAction::getMenu(QWidget* parent /*= nullptr*/)
{
    loadPresetsFromApplicationSettings();

    auto menu = new QMenu("Presets", parent);

    const auto presetIcon = StyledIcon("prescription");

    menu->setIcon(presetIcon);

    auto savePresetAction = new QAction("Save");

    savePresetAction->setIcon(StyledIcon("save"));

    connect(savePresetAction, &TriggerAction::triggered, this, [this, presetIcon]() -> void {
        auto* choosePresetNameDialog = new ChoosePresetNameDialog(this);

        connect(choosePresetNameDialog, &ChoosePresetNameDialog::accepted, this, [this, choosePresetNameDialog]() -> void {
            savePreset(choosePresetNameDialog->getPresetNameAction().getString());
        });

        connect(choosePresetNameDialog, &ChoosePresetNameDialog::finished, choosePresetNameDialog, &ChoosePresetNameDialog::deleteLater);

        choosePresetNameDialog->open();            
    });

    menu->addAction(savePresetAction);

    auto saveDefaultPresetAction = new QAction("Save As Default");

    saveDefaultPresetAction->setIcon(StyledIcon("save"));

    connect(saveDefaultPresetAction, &TriggerAction::triggered, this, &PresetsAction::saveDefaultPreset);

    menu->addAction(saveDefaultPresetAction);

    auto presetNames = _presets.keys();

    if (!presetNames.isEmpty())
        menu->addSeparator();

    if (presetNames.contains("Default")) {
        presetNames.removeOne("Default");
        presetNames.insert(0, "Default");
    }

    for (const auto& presetName : presetNames) {
        auto loadPresetAction = new QAction(presetName);

        loadPresetAction->setIcon(presetIcon);

        connect(loadPresetAction, &TriggerAction::triggered, this, [this, presetName]() -> void {
            loadPreset(presetName);
        });

        menu->addAction(loadPresetAction);

        if (presetName == "Default")
            menu->addSeparator();
    }

    menu->addSeparator();

    auto importPresetAction = new QAction("Import...");
    auto exportPresetAction = new QAction("Export...");

    importPresetAction->setIcon(StyledIcon("file-import"));
    exportPresetAction->setIcon(StyledIcon("file-export"));

    menu->addAction(importPresetAction);
    menu->addAction(exportPresetAction);

    connect(importPresetAction, &TriggerAction::triggered, this, &PresetsAction::importPreset);
    connect(exportPresetAction, &TriggerAction::triggered, this, &PresetsAction::exportPreset);

    menu->addSeparator();
    menu->addAction(&_editAction);

    return menu;
}

void PresetsAction::loadPreset(const QString& name)
{
#ifdef PRESETS_ACTION_VERBOSE
    qDebug() << __FUNCTION__ << name;
#endif

    if (name.isEmpty())
        return;

    if (!_presets.keys().contains(name))
        return;

    emit presetAboutToBeLoaded(name);
    {
        const DatasetPickerAction::ValueSerializationDisabler valueSerializationDisabler;

        _sourceAction->fromVariantMap(_presets[name].toMap());
    }
    emit presetLoaded(name);
}

void PresetsAction::savePreset(const QString& name)
{
#ifdef PRESETS_ACTION_VERBOSE
    qDebug() << __FUNCTION__ << name;
#endif

    if (name.isEmpty())
        return;

    const DatasetPickerAction::ValueSerializationDisabler valueSerializationDisabler;

    auto sourceActionVariantMap = _sourceAction->toVariantMap();

    sourceActionVariantMap["DateTime"] = QDateTime::currentDateTime();

    _presets[name] = sourceActionVariantMap;

    savePresetsToApplicationSettings();

    emit presetSaved(name);
}

void PresetsAction::removePreset(const QString& name)
{
#ifdef PRESETS_ACTION_VERBOSE
    qDebug() << __FUNCTION__ << name;
#endif

    if (!_presets.contains(name))
        return;

    emit presetAboutToBeRemoved(name);
    {
        _presets.remove(name);
        
        savePresetsToApplicationSettings();
        loadPresetsFromApplicationSettings();
    }
    emit presetRemoved(name);
}

void PresetsAction::removePresets(const QStringList& names)
{
#ifdef PRESETS_ACTION_VERBOSE
    qDebug() << __FUNCTION__ << names;
#endif

    if (names.isEmpty())
        return;

    for (const auto& name : names) {
        emit presetAboutToBeRemoved(name);
        {
            if (_presets.contains(name))
                _presets.remove(name);
        }
        emit presetRemoved(name);
    }

    savePresetsToApplicationSettings();
    loadPresetsFromApplicationSettings();
}

void PresetsAction::loadDefaultPreset()
{
    loadPreset("Default");
}

void PresetsAction::saveDefaultPreset()
{
    savePreset("Default");
}

void PresetsAction::importPreset()
{
#ifdef PRESETS_ACTION_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    auto* fileDialog = new QFileDialog();

    fileDialog->setWindowIcon(StyledIcon("file-import"));
    fileDialog->setWindowTitle("Import Preset");
    fileDialog->setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog->setFileMode(QFileDialog::ExistingFile);
    fileDialog->setNameFilters({ "ManiVault View Preset (*.mvp)" });
    fileDialog->setDefaultSuffix(".mvp");
    fileDialog->setOption(QFileDialog::DontUseNativeDialog, true);

    connect(fileDialog, &QFileDialog::accepted, this, [this, fileDialog]() -> void {
        if (fileDialog->selectedFiles().count() != 1)
            throw std::runtime_error("Only one file may be selected");

        const auto presetFilePath = fileDialog->selectedFiles().first();

        _sourceAction->fromJsonFile(presetFilePath);

        savePreset(QFileInfo(presetFilePath).baseName());

        emit presetImported(presetFilePath);
        });
    connect(fileDialog, &QFileDialog::finished, fileDialog, &QFileDialog::deleteLater);

    fileDialog->open();
}

void PresetsAction::exportPreset()
{
#ifdef PRESETS_ACTION_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    auto* fileDialog = new QFileDialog();

    fileDialog->setWindowIcon(StyledIcon("file-export"));
    fileDialog->setWindowTitle("Export Preset");
    fileDialog->setAcceptMode(QFileDialog::AcceptSave);
    fileDialog->setNameFilters({ "ManiVault View Preset (*.mvp)" });
    fileDialog->setDefaultSuffix(".mvp");
    fileDialog->setOption(QFileDialog::DontUseNativeDialog, true);

    connect(fileDialog, &QFileDialog::accepted, this, [this, fileDialog]() -> void {
        if (fileDialog->selectedFiles().count() != 1)
            throw std::runtime_error("Only one file may be selected");

        const auto presetFilePath = fileDialog->selectedFiles().first();

        _sourceAction->toJsonFile(presetFilePath);

        emit presetExported(presetFilePath);
        });
    connect(fileDialog, &QFileDialog::finished, fileDialog, &QFileDialog::deleteLater);

    fileDialog->open();
}

void PresetsAction::updateModel()
{
    _model.setRowCount(0);

    auto presetNames = _presets.keys();

    for (const auto& presetName : presetNames) {
        const auto dateTime = _presets[presetName].toMap()["DateTime"].toDateTime();

        auto nameItem       = new QStandardItem(presetName);
        auto dateTimeItem   = new QStandardItem(dateTime.toString("ddd MMMM d yy"));

        dateTimeItem->setData(dateTime, Qt::EditRole);

        const auto itemEnabled = presetName != "Default";

        nameItem->setEnabled(itemEnabled);
        dateTimeItem->setEnabled(itemEnabled);

        _model.appendRow({ nameItem, dateTimeItem });
    }

    const auto setHeader = [this](Column column) -> void {
        auto headerItem = new QStandardItem(columnInfo[column].first);

        headerItem->setToolTip(columnInfo[column].second);

        _model.setHorizontalHeaderItem(static_cast<int>(column), headerItem);
    };

    setHeader(Column::Name);
    setHeader(Column::DateTime);
}

void PresetsAction::setIcon(const QIcon& icon)
{
    _icon = icon;

    updateModel();
}

const QVariantMap& PresetsAction::getPresets() const
{
    return _presets;
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

PresetsAction::ChoosePresetNameDialog::ChoosePresetNameDialog(PresetsAction* presetsAction, QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _presetNameAction(this, "Preset name"),
    _completer(this),
    _okAction(this, "Create"),
    _cancelAction(this, "Cancel")
{
    setWindowTitle(QString("Choose %1 preset name").arg(presetsAction->getPresetType().toLower()));
    setWindowIcon(StyledIcon("prescription"));

    _completer.setCompletionMode(QCompleter::PopupCompletion);
    _completer.setCaseSensitivity(Qt::CaseInsensitive);
    _completer.setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    _completer.setModel(new QStringListModel(presetsAction->getPresets().keys()));

    _presetNameAction.setCompleter(&_completer);
    _presetNameAction.setClearable(true);
    _presetNameAction.setPlaceHolderString("Enter preset name here...");

    auto mainLayout = new QGridLayout();

    mainLayout->addWidget(_presetNameAction.createLabelWidget(this), 0, 0);
    mainLayout->addWidget(_presetNameAction.createWidget(this), 0, 1);

    auto buttonsLayout = new QHBoxLayout();

    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(_okAction.createWidget(this));
    buttonsLayout->addWidget(_cancelAction.createWidget(this));

    mainLayout->addLayout(buttonsLayout, 1, 0, 1, 2);

    setLayout(mainLayout);

    const auto updateOkAction = [this]() -> void {
        _okAction.setEnabled(!_presetNameAction.getString().isEmpty());
    };

    connect(&_presetNameAction, &StringAction::stringChanged, this, updateOkAction);
    connect(&_okAction, &TriggerAction::triggered, this, &QDialog::accept);
    connect(&_cancelAction, &TriggerAction::triggered, this, &QDialog::reject);

    updateOkAction();
}

QSize PresetsAction::ChoosePresetNameDialog::sizeHint() const
{
    return { 300, 0 };
}

PresetsAction::ManagePresetsDialog::ManagePresetsDialog(PresetsAction* presetsAction) :
    _hierarchyWidget(this, QString("%1 Preset").arg(presetsAction->getPresetType()), presetsAction->getModel(), const_cast<PresetsAction::FilterModel*>(&presetsAction->getFilterModel())),
    _removeAction(this, "Remove"),
    _okAction(this, "Ok")
{
    setModal(true);
    setWindowIcon(StyledIcon("prescription"));
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

    _hierarchyWidget.setWindowIcon(windowIcon());
    _hierarchyWidget.getExpandAllAction().setVisible(false);
    _hierarchyWidget.getCollapseAllAction().setVisible(false);

    auto treeViewHeader = treeView.header();

    treeViewHeader->setStretchLastSection(false);

    treeViewHeader->setSortIndicator(static_cast<int>(Column::Name), Qt::DescendingOrder);

    treeViewHeader->resizeSection(static_cast<int>(Column::DateTime), 100);

    treeViewHeader->setSectionResizeMode(static_cast<int>(Column::Name), QHeaderView::Stretch);
    treeViewHeader->setSectionResizeMode(static_cast<int>(Column::DateTime), QHeaderView::Fixed);

    _removeAction.setEnabled(false);
    
    const auto updateRemoveReadOnly = [this, presetsAction]() -> void {
        const auto numberOfSelectedRows = _hierarchyWidget.getTreeView().selectionModel()->selectedRows().count();

        _removeAction.setText(QString("Remove %1").arg(numberOfSelectedRows >= 1 ? QString::number(numberOfSelectedRows) : ""));
        _removeAction.setToolTip(QString("Remove %1 recent %2%3").arg(numberOfSelectedRows >= 1 ? QString::number(numberOfSelectedRows) : "", presetsAction->getPresetType().toLower(), numberOfSelectedRows >= 2 ? "s" : ""));
        _removeAction.setEnabled(numberOfSelectedRows >= 1);
    };

    connect(_hierarchyWidget.getTreeView().selectionModel(), &QItemSelectionModel::selectionChanged, this, updateRemoveReadOnly);
    connect(&presetsAction->getFilterModel(), &QAbstractItemModel::rowsInserted, this, updateRemoveReadOnly);
    connect(&presetsAction->getFilterModel(), &QAbstractItemModel::rowsRemoved, this, updateRemoveReadOnly);

    connect(&_removeAction, &TriggerAction::triggered, this, [this, presetsAction]() -> void {
        const auto selectedRows = _hierarchyWidget.getTreeView().selectionModel()->selectedRows();

        if (selectedRows.isEmpty())
            return;

        QStringList presetsToRemove;

        for (const auto& selectedRow : selectedRows)
            presetsToRemove << selectedRow.siblingAtColumn(static_cast<int>(Column::Name)).data().toString();

        presetsAction->removePresets(presetsToRemove);
    });

    connect(&_okAction, &TriggerAction::triggered, this, &ManagePresetsDialog::accept);
}

QSize PresetsAction::ManagePresetsDialog::sizeHint() const
{
    return { 480, 480 };
}

}
