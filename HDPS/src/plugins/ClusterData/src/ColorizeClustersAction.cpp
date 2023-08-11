// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ColorizeClustersAction.h"
#include "ClustersAction.h"
#include "ClustersFilterModel.h"
#include "ClusterData.h"
#include "ClustersActionWidget.h"

ColorizeClustersAction::ColorizeClustersAction(ClustersAction& clustersAction) :
    TriggerAction(&clustersAction, "Colorize"),
    _clustersAction(clustersAction),
    _colorByAction(this, "Color by", {"Color map", "Pseudo-random colors"}, "Color map"),
    _colorMapAction(this, "Color map"),
    _randomSeedAction(this, "Random color seed"),
    _colorizeAction(this, "Colorize")
{
    setToolTip("Colorize clusters");
    setIconByName("palette");

    // Do not update when dragging the pseudo-random seed slider
    _randomSeedAction.setUpdateDuringDrag(false);

    // Disable color map range editing
    _colorMapAction.getRangeAction(ColorMapAction::Axis::X).setEnabled(false);

    // Update action read only status
    const auto updateReadOnly = [this]() -> void {
        const auto colorBy          = _colorByAction.getCurrentIndex();
        const auto numberOfClusters = _clustersAction.getClustersModel().rowCount();
        const auto hasClusters      = numberOfClusters >= 1;

        setEnabled(hasClusters);

        _colorByAction.setEnabled(hasClusters);
        _colorMapAction.setEnabled(_colorByAction.getCurrentText() == "Color map" && hasClusters);
        _randomSeedAction.setEnabled(_colorByAction.getCurrentText() == "Pseudo-random colors" && hasClusters);
    };

    // Generate the cluster colors in the model
    const auto updateColorsInModel = [this]() -> void {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        {
            switch (_colorByAction.getCurrentIndex())
            {
                // Color map
                case 0:
                    _clustersAction.getClustersModel().colorizeClusters(_colorMapAction.getColorMapImage());
                    break;

                // Pseudo-random colors
                case 1:
                    _clustersAction.getClustersModel().colorizeClusters(_randomSeedAction.getValue());
                    break;

                default:
                    break;
            }
        }
        QApplication::restoreOverrideCursor();
    };

    // Update the cluster colors in the model when actions change
    connect(&_colorByAction, &OptionAction::currentIndexChanged, this, updateColorsInModel);
    connect(&_colorMapAction, &ColorMapAction::imageChanged, this, updateColorsInModel);
    connect(&_randomSeedAction, &IntegralAction::valueChanged, this, updateColorsInModel);
    connect(&_colorizeAction, &TriggerAction::triggered, this, updateColorsInModel);

    // Update read only status when the model layout changes
    connect(&_clustersAction.getClustersModel(), &QAbstractItemModel::layoutChanged, this, updateReadOnly);

    // Update read only status when the color by action changes
    connect(&_colorByAction, &OptionAction::currentIndexChanged, this, updateReadOnly);

    // Initialize read only status
    updateReadOnly();
}

ColorizeClustersAction::Widget::Widget(QWidget* parent, ColorizeClustersAction* colorizeClustersAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, colorizeClustersAction)
{
    auto layout = new QGridLayout();

    layout->addWidget(colorizeClustersAction->getColorByAction().createLabelWidget(this), 0, 0);
    layout->addWidget(colorizeClustersAction->getColorByAction().createWidget(this), 0, 1);

    layout->addWidget(colorizeClustersAction->getColorMapAction().createLabelWidget(this), 1, 0);
    layout->addWidget(colorizeClustersAction->getColorMapAction().createWidget(this), 1, 1);

    layout->addWidget(colorizeClustersAction->getRandomSeedAction().createLabelWidget(this), 2, 0);
    layout->addWidget(colorizeClustersAction->getRandomSeedAction().createWidget(this), 2, 1);

    setLayout(layout);
}
