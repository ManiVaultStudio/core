// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <actions/TriggerAction.h>
#include <actions/OptionAction.h>
#include <actions/ColorMap1DAction.h>
#include <actions/IntegralAction.h>
#include <actions/TriggerAction.h>

using namespace hdps;
using namespace hdps::gui;

class ClustersAction;

/**
 * Colorize clusters action class
 *
 * Action class for coloring clusters
 *
 * @author Thomas Kroes
 */
class ColorizeClustersAction : public TriggerAction
{
protected:

    /** Widget class for colorize clusters action */
    class Widget : public WidgetActionWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param colorizeClustersAction Pointer to colorize clusters action
         * @param widgetFlags Widget flags for the configuration of the widget
         */
        Widget(QWidget* parent, ColorizeClustersAction* colorizeClustersAction, const std::int32_t& widgetFlags);
    };

    /**
     * Get widget representation of the colorize clusters action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this, widgetFlags);
    };

public:

    /**
     * Constructor
     * @param clustersAction Reference to clusters action
     */
    ColorizeClustersAction(ClustersAction& clustersAction);

public: // Action getters

    OptionAction& getColorByAction() { return _colorByAction; }
    ColorMapAction& getColorMapAction() { return _colorMapAction; }
    IntegralAction& getRandomSeedAction() { return _randomSeedAction; }
    TriggerAction& getColorizeAction() { return _colorizeAction; }

public slots:
    /** When the color model or cluster number changes, update the cluster colors */
    void updateColorsInModel();

protected:
    ClustersAction&     _clustersAction;        /** Reference to clusters action */
    OptionAction        _colorByAction;         /** Color by action */
    ColorMap1DAction    _colorMapAction;        /** Color map action */
    IntegralAction      _randomSeedAction;      /** Random seed action for random color generation */
    TriggerAction       _colorizeAction;        /** Colorize action */
};