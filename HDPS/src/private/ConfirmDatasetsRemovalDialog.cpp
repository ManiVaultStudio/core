// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ConfirmDatasetsRemovalDialog.h"

#include <CoreInterface.h>
#include <Application.h>
#include <Set.h>

#include <QVBoxLayout>
#include <QLabel>
#include <QHeaderView>

using namespace mv;

ConfirmDatasetsRemovalDialog::ConfirmDatasetsRemovalDialog(mv::Datasets selectedDatasets, QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _selectedDatasets(selectedDatasets),
    _datasetsToRemoveModel(),
    _datasetsToRemoveFilterModel(),
    _mainGroupAction(this, "Main group"),
    _messageAction(this, "Message"),
    _descendantsModeAction(this, "Descendants", { "Keep", "Remove"}),
    _selectDatasetsGroupAction(this, "Select datasets to remove"),
    _selectDatasetsAction(this, "Select datasets"),
    _bottomHorizontalGroupAction(this, "Bottom group"),
    _showAgainAction(this, "Show again", true),
    _removeAction(this, "Remove"),
    _cancelAction(this, "Cancel")
{
    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("trash"));
    setWindowTitle("About to remove dataset(s)");
    setModal(true);

    _datasetsToRemoveModel.setDatasets(_selectedDatasets);

    _mainGroupAction.setShowLabels(false);

    _mainGroupAction.addAction(&_messageAction);
    _mainGroupAction.addAction(&_descendantsModeAction);
    _mainGroupAction.addStretch();
    _mainGroupAction.addAction(&_bottomHorizontalGroupAction);

    _messageAction.setDefaultWidgetFlags(StringAction::WidgetFlag::Label);
    _messageAction.setString(QString("The selected dataset%1 contain%2 children\n, what would you like to do with them?").arg(_selectedDatasets.count() == 1 ? "" : "s", _selectedDatasets.count() == 1 ? "s" : ""));

    _descendantsModeAction.setDefaultWidgetFlags(OptionAction::WidgetFlag::HorizontalButtons);
    _descendantsModeAction.setCurrentIndex(settings().getMiscellaneousSettings().getKeepDescendantsAfterRemovalAction().isChecked() ? 0 : 1);

    connect(&_descendantsModeAction, &OptionAction::currentIndexChanged, this, [this](const std::int32_t& currentIndex) -> void {
        settings().getMiscellaneousSettings().getKeepDescendantsAfterRemovalAction().setChecked(currentIndex == 0);
    });

    _selectDatasetsAction.initialize(&_datasetsToRemoveModel, &_datasetsToRemoveFilterModel, "dataset");

    _selectDatasetsGroupAction.setIconByName("mouse-pointer");
    _selectDatasetsGroupAction.setShowLabels(false);
    _selectDatasetsGroupAction.setPopupSizeHint(QSize(400, 400));
    _selectDatasetsGroupAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);

    _selectDatasetsGroupAction.addAction(&_descendantsModeAction);
    _selectDatasetsGroupAction.addAction(&_selectDatasetsAction, -1, [this](WidgetAction* action, QWidget* widget) -> void {
        auto hierarchyWidget = widget->findChild<HierarchyWidget*>("HierarchyWidget");

        Q_ASSERT(hierarchyWidget != nullptr);

        if (hierarchyWidget == nullptr)
            return;

        hierarchyWidget->setWindowIcon(Application::getIconFont("FontAwesome").getIcon("database"));

        auto treeView = widget->findChild<QTreeView*>("TreeView");

        Q_ASSERT(treeView != nullptr);

        if (treeView == nullptr)
            return;

        treeView->expandAll();
        treeView->setColumnHidden(static_cast<int>(DatasetsToRemoveModel::Column::DatasetId), true);

        auto treeViewHeader = treeView->header();

        treeViewHeader->setStretchLastSection(false);

        treeViewHeader->setSectionResizeMode(static_cast<int>(DatasetsToRemoveModel::Column::Name), QHeaderView::Stretch);
        treeViewHeader->setSectionResizeMode(static_cast<int>(DatasetsToRemoveModel::Column::DatasetId), QHeaderView::Stretch);

        const auto resizeSections = [this, treeViewHeader]() -> void {
            if (_datasetsToRemoveFilterModel.rowCount() >= 1)
                treeViewHeader->resizeSections(QHeaderView::ResizeMode::ResizeToContents);
        };

        resizeSections();

        connect(&_datasetsToRemoveFilterModel, &QAbstractItemModel::rowsInserted, widget, resizeSections);
        connect(&_datasetsToRemoveFilterModel, &QAbstractItemModel::rowsRemoved, widget, resizeSections);
        connect(&_datasetsToRemoveFilterModel, &QAbstractItemModel::layoutChanged, widget, resizeSections);
    });

    _bottomHorizontalGroupAction.setShowLabels(false);

    _bottomHorizontalGroupAction.addAction(&_showAgainAction);
    _bottomHorizontalGroupAction.addStretch();
    _bottomHorizontalGroupAction.addAction(&_selectDatasetsGroupAction);
    _bottomHorizontalGroupAction.addAction(&_removeAction);
    _bottomHorizontalGroupAction.addAction(&_cancelAction);

    _showAgainAction.setChecked(settings().getMiscellaneousSettings().getAskConfirmationBeforeRemovingDatasetsAction().isChecked());

    connect(&_showAgainAction, &ToggleAction::toggled, this, [this](bool toggled) -> void {
        settings().getMiscellaneousSettings().getAskConfirmationBeforeRemovingDatasetsAction().setChecked(toggled);
    });

    _removeAction.setToolTip("Remove the dataset(s)");
    _cancelAction.setToolTip("Do not remove the dataset(s) and quit this dialog");

    _updateRemoveActionTimer.setSingleShot(true);
    _updateRemoveActionTimer.setInterval(10);

    const auto updateRemoveAction = [this]() -> void {
        const auto numberOfDatasetsToRemove = getDatasetsToRemove().count();

        _removeAction.setEnabled(numberOfDatasetsToRemove >= 1);

        if (numberOfDatasetsToRemove == 0)
            _removeAction.setText("Remove");
        else
            _removeAction.setText(QString("Remove %1").arg(QString::number(numberOfDatasetsToRemove)));
    };

    const auto modelChanged = [this, updateRemoveAction]() -> void {
        if (!_updateRemoveActionTimer.isActive()) {
            updateRemoveAction();
            _updateRemoveActionTimer.start();
        }
        else {
            _updateRemoveActionTimer.start();
        }
    };

    connect(&_updateRemoveActionTimer, &QTimer::timeout, this, updateRemoveAction);

    updateRemoveAction();

    connect(&_datasetsToRemoveFilterModel, &QAbstractItemModel::rowsInserted, this, modelChanged);
    connect(&_datasetsToRemoveFilterModel, &QAbstractItemModel::rowsRemoved, this, modelChanged);
    connect(&_datasetsToRemoveFilterModel, &QAbstractItemModel::layoutChanged, this, modelChanged);
    connect(&_datasetsToRemoveFilterModel, &QAbstractItemModel::dataChanged, this, modelChanged);

    connect(&_removeAction, &TriggerAction::triggered, this, &ConfirmDatasetsRemovalDialog::accept);
    connect(&_cancelAction, &TriggerAction::triggered, this, &ConfirmDatasetsRemovalDialog::reject);

    auto layout = new QVBoxLayout();

    setLayout(layout);

    layout->addWidget(_mainGroupAction.createWidget(this));
}

mv::Datasets ConfirmDatasetsRemovalDialog::getDatasetsToRemove() const
{
    return _datasetsToRemoveModel.getDatasetsToRemove();
}
