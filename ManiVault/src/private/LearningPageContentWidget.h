// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "PageContentWidget.h"
#include "PageTutorialsWidget.h"

#include "LearningPageVideosWidget.h"
#include "LearningPageExamplesWidget.h"
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
    mv::gui::ToggleAction               _showVideosAction;              /** Action to toggle videos section */
    mv::gui::ToggleAction               _showTutorialsAction;           /** Action to toggle tutorials section */
    mv::gui::ToggleAction               _showExamplesAction;            /** Action to toggle examples section */
    mv::gui::ToggleAction               _showPluginResourcesAction;     /** Action to toggle plugin resources section */
    mv::gui::GroupAction                _settingsAction;                /** Page settings action */
    mv::gui::TriggerAction              _toStartPageAction;             /** Trigger action for showing the start page */
    mv::gui::HorizontalGroupAction      _toolbarAction;                 /** Bottom toolbar action */
    LearningPageVideosWidget            _videosWidget;                  /** Learning page video section content widget */
    PageTutorialsWidget                 _tutorialsWidget;               /** Learning page tutorials section content widget */
    LearningPageExamplesWidget          _examplesWidget;                /** Learning page examples section content widget */
    LearningPagePluginResourcesWidget   _pluginResourcesWidget;         /** Learning page plugin section resources content widget */

    friend class LearningPageWidget;
};