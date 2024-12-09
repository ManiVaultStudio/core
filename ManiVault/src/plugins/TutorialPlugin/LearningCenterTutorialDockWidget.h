// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <DockWidget.h>

#include <util/LearningCenterTutorial.h>

#include <QWebEngineView>

/**
 * Learning center tutorial dock widget class
 *
 * @author Thomas Kroes
 */
class LearningCenterTutorialDockWidget : public ads::CDockWidget
{
    Q_OBJECT

public:

    /**
     * Construct with reference to \p tutorial and pointer to \p parent widget
     * @param tutorial Reference to the learning center tutorials
     * @param parent Pointer to parent widget
     */
    LearningCenterTutorialDockWidget(const mv::util::LearningCenterTutorial& tutorial, QWidget* parent = nullptr);

private:
    const mv::util::LearningCenterTutorial&     _learningCenterTutorial;    /** Learning center tutorial to display */
    QWebEngineView                              _tutorialView;              /** To display the HTML tutorial */
};

