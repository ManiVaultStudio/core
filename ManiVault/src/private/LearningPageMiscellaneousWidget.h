// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QWidget>

class LearningPageContentWidget;

/**
 * Learning page miscellaneous widget class
 *
 * Widget class which lists learning page miscellaneous content
 *
 * @author Thomas Kroes
 */
class LearningPageMiscellaneousWidget : public QWidget
{
protected:

    /**
     * Construct with pointer to \p learningPageContentWidget
     * @param learningPageContentWidget Pointer to owning learning page content widget
     */
    LearningPageMiscellaneousWidget(LearningPageContentWidget* learningPageContentWidget);

private:
    LearningPageContentWidget* _learningPageContentWidget;  /** Pointer to owning learning page content widget */

    friend class LearningPageContentWidget;
};