// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "PageContentWidget.h"

#include <actions/TriggerAction.h>

#include "StartPageOpenProjectWidget.h"
#include "StartPageGetStartedWidget.h"

/**
 * Start page content widget class
 *
 * For showing start page actions.
 *
 * @author Thomas Kroes
 */
class StartPageContentWidget final : public PageContentWidget
{
protected:

    /**
     * Construct with \p parent widget
     * @param parent Pointer to parent widget
     */
    StartPageContentWidget(QWidget* parent = nullptr);

    /** Updates all actions */
    void updateActions();

public: // Action getters

    const mv::gui::TriggerAction& getToLearningCenterAction() const { return _toLearningCenterAction; }

    mv::gui::HorizontalGroupAction& getToolbarAction() { return _toolbarAction; }

private:
    mv::gui::TriggerAction          _toLearningCenterAction;            /** Trigger action for showing the learning center */
    mv::gui::HorizontalGroupAction  _toolbarAction;                     /** Bottom toolbar action */
    StartPageOpenProjectWidget      _openProjectWidget;                 /** Left column widget for opening existing projects */
    StartPageGetStartedWidget       _getStartedWidget;                  /** Right column widget for getting started operations */

    friend class StartPageWidget;
};