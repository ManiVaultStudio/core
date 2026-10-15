// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <actions/TriggerAction.h>
#include <actions/OptionAction.h>
#include <actions/ColorMap1DAction.h>
#include <actions/IntegralAction.h>
#include <actions/TriggerAction.h>

class ClustersAction;

/**
 * Colorize clusters action class
 *
 * Action class for coloring clusters
 *
 * @author Thomas Kroes
 */
class ColorizeClustersAction : public mv::gui::TriggerAction
{
protected:

    /** Widget class for colorize clusters action */
    class Widget : public mv::gui::WidgetActionWidget {
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

    mv::gui::OptionAction& getColorByAction() { return _colorByAction; }
    mv::gui::ColorMapAction& getColorMapAction() { return _colorMapAction; }
    mv::gui::IntegralAction& getRandomSeedAction() { return _randomSeedAction; }
    mv::gui::TriggerAction& getColorizeAction() { return _colorizeAction; }

public slots:
    /** When the color model or cluster number changes, update the cluster colors */
    void updateColorsInModel();

protected:
    ClustersAction&             _clustersAction;        /** Reference to clusters action */
    mv::gui::OptionAction       _colorByAction;         /** Color by action */
    mv::gui::ColorMap1DAction   _colorMapAction;        /** Color map action */
    mv::gui::IntegralAction     _randomSeedAction;      /** Random seed action for random color generation */
    mv::gui::TriggerAction      _colorizeAction;        /** Colorize action */
};