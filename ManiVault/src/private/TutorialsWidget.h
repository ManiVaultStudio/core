// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "PageActionsWidget.h"

#include <models/LearningCenterTutorialsFilterModel.h>

#include <QVBoxLayout>
#include <QWidget>

class LearningPageContentWidget;

/**
 * Page tutorials class
 *
 * Page tutorials content
 *
 * @author Thomas Kroes
 */
class LearningPageTutorialsWidget : public PageActionsWidget
{
protected:

    /**
     * Construct with pointer to \p learningPageContentWidget
     * @param learningPageContentWidget Pointer to owning learning page content widget
     */
    LearningPageTutorialsWidget(LearningPageContentWidget* learningPageContentWidget);

protected:
    /**
     * Override QObject's event handling
     * @return Boolean Wheter the event was recognized and processed
     */
    bool event(QEvent* event) override;

private:

    /** Update all custom style elements */
    void updateCustomStyle();

private:
    LearningPageContentWidget*                  _learningPageContentWidget;             /** Pointer to owning learning page content widget */
    QVBoxLayout                                 _mainLayout;                            /** Main vertical layout */
    mv::LearningCenterTutorialsFilterModel      _learningCenterTutorialsFilterModel;    /** For filtering learning center tutorials */
    PageActionsWidget                           _tutorialsWidget;                       /** Actions widget for launching tutorials */

    friend class LearningPageContentWidget;
};
