// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "PageContentWidget.h"
#include "LearningPageVideosWidget.h"
#include "LearningPagePluginResourcesWidget.h"

/**
 * Learning page content widget class
 *
 * Widget class for showing all learning page content
 *
 * @author Thomas Kroes
 */
class LearningPageContentWidget final : public PageContentWidget
{
protected:

    /**
     * Construct with pointer to \p parent widget
     * @param parent Pointer to parent widget
     */
    LearningPageContentWidget(QWidget* parent = nullptr);

private:
    mv::gui::ToggleAction               _showVideosAction;              /** Toggle videos action */
    mv::gui::ToggleAction               _showTutorialsAction;           /** Toggle tutorials action */
    mv::gui::ToggleAction               _showPluginResourcesAction;     /** Toggle plugin resources action */
    mv::gui::GroupAction                _settingsAction;                /** Page settings action */
    mv::gui::TriggerAction              _toStartPageAction;             /** Trigger action for showing the start page */
    mv::gui::HorizontalGroupAction      _toolbarAction;                 /** Bottom toolbar action */
    LearningPageVideosWidget            _videosWidget;                  /** Learning page video content widget */
    LearningPagePluginResourcesWidget   _pluginResourcesWidget;         /** Learning page plugin resources content widget */

    friend class LearningPageWidget;
};