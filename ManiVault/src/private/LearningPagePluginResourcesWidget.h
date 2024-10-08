// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QWidget>
#include <QVBoxLayout>

#include <actions/OptionsAction.h> 
#include <widgets/FlowLayout.h> 

class LearningPageContentWidget;

/**
 * Learning page miscellaneous widget class
 *
 * Widget class which lists learning page miscellaneous content
 *
 * @author Thomas Kroes
 */
class LearningPagePluginResourcesWidget : public QWidget
{
protected:

    /**
     * Construct with pointer to \p learningPageContentWidget
     * @param learningPageContentWidget Pointer to owning learning page content widget
     */
    LearningPagePluginResourcesWidget(LearningPageContentWidget* learningPageContentWidget);

private:

    /** Updates the displayed plugin(s) based on the current filtering */
    void updateFlowLayout();

private:
    LearningPageContentWidget*      _learningPageContentWidget;     /** Pointer to owning learning page content widget */
    QVBoxLayout                     _mainLayout;                    /** Main vertical layout */
    mv::gui::OptionsAction          _pluginTypesFilterAction;       /** Filter based on plugin types */
    mv::gui::FlowLayout             _pluginsLayout;                 /** Show plugins side-by-side */

    friend class LearningPageContentWidget;
};
