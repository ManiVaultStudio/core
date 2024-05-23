// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "PageWidget.h"
#include "LearningPageContentWidget.h"

#include <QWidget>

/**
 * Learning page widget class
 *
 * Widget class which displays learning page content
 *
 * @author Thomas Kroes
 */
class LearningPageWidget : public PageWidget
{
public:

    /**
     * Construct with pointer to \p parent widget
     * @param parent Pointer to parent widget
     */
    LearningPageWidget(QWidget* parent = nullptr);

protected:
    LearningPageContentWidget  _contentWidget;    /** Widget which displays all the learning page content */
};
