// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "PageActionsWidget.h"

#include <actions/HorizontalGroupAction.h>

#include <models/LearningCenterTutorialsFilterModel.h>

class LearningPageContentWidget;

/**
 * Page tutorials class
 *
 * Tutorials content
 *
 * @author Thomas Kroes
 */
class PageTutorialsWidget : public PageActionsWidget
{
public:

    /**
     * Construct with pointer to \p parent widget
     * @param parent Pointer to parent widget
     * @param tags Initial tags filter
     */
    PageTutorialsWidget(QWidget* parent, const QStringList& tags = QStringList());

    /**
     * Get toolbar action
     * @return Reference to toolbar action
     */
    mv::gui::HorizontalGroupAction& getToolbarAction();

    /**
     * Get learning center tutorials filter model
     * @return Reference to learning center tutorials filter model
     */
    mv::LearningCenterTutorialsFilterModel& getTutorialsFilterModel();

protected:
    /**
     * Override QObject's event handling
     * @return Boolean Whether the event was recognized and processed
     */
    bool event(QEvent* event) override;

private:

    /** Update actions for launching tutorials */
    void updateActions();

    /** Update all custom style elements */
    void updateCustomStyle();

private:
    mv::gui::HorizontalGroupAction              _toolbarAction;             /** Horizontal toolbar (group) action */
    mv::LearningCenterTutorialsFilterModel      _tutorialsFilterModel;      /** For filtering learning center tutorials */
};
