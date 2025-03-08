// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QWidget>
#include <QVBoxLayout>

class LearningPageContentWidget;

/**
 * Learning page tutorials widget class
 *
 * Widget class which lists learning page tutorials content
 *
 * @author Thomas Kroes
 */
class LearningPageExamplesWidget : public QWidget
{
protected:

    /**
     * Construct with pointer to \p learningPageContentWidget
     * @param learningPageContentWidget Pointer to owning learning page content widget
     */
    LearningPageExamplesWidget(LearningPageContentWidget* learningPageContentWidget);

private:

    /** Update all custom style elements */
    void updateCustomStyle();

protected:
    /**
     * Override QObject's event handling
     * @return Boolean Whether the event was recognized and processed
     */
    bool event(QEvent* event) override;

private:
    LearningPageContentWidget*  _learningPageContentWidget;     /** Pointer to owning learning page content widget */
    QVBoxLayout                 _mainLayout;                    /** Main vertical layout */

    friend class LearningPageContentWidget;
};
