// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DimensionsPickerSelectAction.h"
#include "DimensionsPickerAction.h"

#include <Application.h>

#include <QLabel>
#include <QVBoxLayout>

#include "../../../widgets/FileDialog.h"

namespace mv::gui {

DimensionsPickerSelectAction::DimensionsPickerSelectAction(DimensionsPickerAction& dimensionsPickerAction) :
    WidgetAction(&dimensionsPickerAction, "Dimension Picker"),
    _dimensionsPickerAction(dimensionsPickerAction),
    _selectionThresholdAction(this, "Selection threshold", 0),
    _computeStatisticsAction(this, "Compute statistics"),
    _selectVisibleAction(this, "Select visible"),
    _selectNonVisibleAction(this, "Select non-visible"),
    _loadSelectionAction(this, "Load selection"),
    _saveSelectionAction(this, "Save selection"),
    _loadExclusionAction(this, "Load exclusion")
{
    setText("Select");
    setIconByName("arrow-pointer");

    //_selectionThresholdAction.setEnabled(false);

    _selectionThresholdAction.setToolTip("Threshold for selecting dimensions");
    _computeStatisticsAction.setToolTip("Compute the dimension statistics");
    _selectVisibleAction.setToolTip("Select visible dimensions");
    _selectNonVisibleAction.setToolTip("Select non-visible dimensions");
    _loadSelectionAction.setToolTip("Load dimension selection from file");
    _saveSelectionAction.setToolTip("Save dimension selection to file");
    _loadExclusionAction.setToolTip("Load dimension exclusion selection to file");

    _selectVisibleAction.setIconByName("eye");
    _selectNonVisibleAction.setIconByName("eye-slash");

    _selectVisibleAction.setDefaultWidgetFlags(TriggerAction::Icon);
    _selectNonVisibleAction.setDefaultWidgetFlags(TriggerAction::Icon);

    const auto selectionFileFilter = tr("Text files (*.txt);;All files (*.*)");

    // Update the visible dimensions when the selection threshold changes
    connect(&_selectionThresholdAction, &IntegralAction::valueChanged, this, [this](const std::int32_t& value) {
        const auto sliderMaximum = _selectionThresholdAction.getMaximum();

        if (value >= 0 && value <= sliderMaximum)
        {
            const auto& distinctStandardDeviations = _dimensionsPickerAction.getHolder().distinctStandardDeviationsWithAndWithoutZero[_dimensionsPickerAction.getHolder()._ignoreZeroValues ? 1 : 0];
            const auto numberOfDistinctStandardDeviations = distinctStandardDeviations.size();

            if (numberOfDistinctStandardDeviations > 0 && sliderMaximum > 0)
            {
                if ((sliderMaximum + 1) == numberOfDistinctStandardDeviations)
                {
                    const ModelResetter modelResetter(&_dimensionsPickerAction.getProxyModel());

                    _dimensionsPickerAction.getProxyModel().SetMinimumStandardDeviation(distinctStandardDeviations[value]);
                }
                else
                    assert(!"Slider maximum incorrect!");
            }
        }
        else
            assert(!"Slider value out of range!");
    });

    // Select visible dimensions when the corresponding action is triggered
    connect(&_selectVisibleAction, &TriggerAction::triggered, this, [this]() {
        _dimensionsPickerAction.selectDimensionsBasedOnVisibility<true>();
    });

    // Select non-visible dimensions when the corresponding action is triggered
    connect(&_selectNonVisibleAction, &TriggerAction::triggered, this, [this]() {
        _dimensionsPickerAction.selectDimensionsBasedOnVisibility<false>();
    });

    // Load dimension selection from file when the corresponding action is triggered
    connect(&_loadSelectionAction, &TriggerAction::triggered, this, [this, selectionFileFilter]() {
        // Don't call fileDialog.exec() or use a static method like QFileDialog::getOpenFileName since they trigger some assertion failures due to threading issues when opening the dialog here
        auto fileOpenDialog = new FileOpenDialog(nullptr, tr("Load dimension selection from file"), {}, selectionFileFilter);

        fileOpenDialog->setFileMode(QFileDialog::ExistingFile);

        connect(fileOpenDialog, &QFileDialog::accepted, this, [this, fileOpenDialog]() -> void {
            _dimensionsPickerAction.loadSelectionFromFile(fileOpenDialog->selectedFiles().first());
            fileOpenDialog->deleteLater();
        });

        connect(fileOpenDialog, &QFileDialog::rejected, this, [this, fileOpenDialog]() -> void {
            fileOpenDialog->deleteLater();
        });

        fileOpenDialog->open();
    });

    // Save dimension selection to file when the corresponding action is triggered
    connect(&_saveSelectionAction, &TriggerAction::triggered, this, [this, selectionFileFilter]() {
        auto fileSaveDialog = new FileSaveDialog(nullptr, tr("Save dimension selection to file"), {}, selectionFileFilter);

        fileSaveDialog->setFileMode(QFileDialog::AnyFile);

        connect(fileSaveDialog, &QFileDialog::accepted, this, [this, fileSaveDialog]() -> void {
            QString fileName = fileSaveDialog->selectedFiles().first();;
            _dimensionsPickerAction.saveSelectionToFile(fileName);

            fileSaveDialog->deleteLater();
        });

        connect(fileSaveDialog, &QFileDialog::rejected, this, [this, fileSaveDialog]() -> void {
            fileSaveDialog->deleteLater();
        });

        fileSaveDialog->open();
    });

    // Load dimension exclusion from file when the corresponding action is triggered
    connect(&_loadExclusionAction, &TriggerAction::triggered, this, [this, selectionFileFilter]() {
        QFileDialog* fileDialog = new QFileDialog(nullptr, tr("Load dimension exclusion list from file"), {}, selectionFileFilter);
        fileDialog->setAcceptMode(QFileDialog::AcceptOpen);
        fileDialog->setFileMode(QFileDialog::ExistingFile);

        connect(fileDialog, &QFileDialog::accepted, this, [this, fileDialog]() -> void {
            QString fileName = fileDialog->selectedFiles().first();;
            _dimensionsPickerAction.loadExclusionFromFile(fileName);
            fileDialog->deleteLater();
            });

        fileDialog->open();
    });
}

DimensionsPickerSelectAction::Widget::Widget(QWidget* parent, DimensionsPickerSelectAction* dimensionsPickerSelectAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, dimensionsPickerSelectAction, widgetFlags)
{
    auto layout = new QVBoxLayout();

    auto selectionThresholdLayout = new QHBoxLayout();

    auto moreLabel = new QLabel("More");
    auto lessLabel = new QLabel("Less");

    selectionThresholdLayout->addWidget(moreLabel);
    selectionThresholdLayout->addWidget(dimensionsPickerSelectAction->getSelectionThresholdAction().createWidget(this, IntegralAction::Slider));
    selectionThresholdLayout->addWidget(lessLabel);

    // Synchronize read-only status of the threshold labels with the threshold action
    const auto updateSelectionThresholdLabels = [this, dimensionsPickerSelectAction, moreLabel, lessLabel]() -> void {
        const auto isEnabled = dimensionsPickerSelectAction->getSelectionThresholdAction().isEnabled();

        moreLabel->setEnabled(isEnabled);
        lessLabel->setEnabled(isEnabled);
    };

    // Synchronize read-only status of the threshold labels when the selection threshold changes
    connect(&dimensionsPickerSelectAction->getSelectionThresholdAction(), &IntegralAction::changed, this, [this, updateSelectionThresholdLabels]() {
        updateSelectionThresholdLabels();
    });

    // Initial synchronization of the read-only status of the threshold labels
    updateSelectionThresholdLabels();

    layout->addLayout(selectionThresholdLayout);

    layout->addWidget(dimensionsPickerSelectAction->getComputeStatisticsAction().createWidget(this));

    auto selectLayout = new QHBoxLayout();

    selectLayout->addWidget(dimensionsPickerSelectAction->getSelectVisibleAction().createWidget(this));
    selectLayout->addWidget(dimensionsPickerSelectAction->getSelectNonVisibleAction().createWidget(this));
    selectLayout->addWidget(dimensionsPickerSelectAction->getLoadSelectionAction().createWidget(this));
    selectLayout->addWidget(dimensionsPickerSelectAction->getSaveSelectionAction().createWidget(this));
    selectLayout->addWidget(dimensionsPickerSelectAction->getLoadExclusionAction().createWidget(this));

    layout->addLayout(selectLayout);

    setLayout(layout);
}

}



