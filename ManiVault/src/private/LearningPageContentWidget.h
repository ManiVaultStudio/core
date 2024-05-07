// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "PageContentWidget.h"
#include "LearningPageVideosWidget.h"
#include "LearningPageMiscellaneousWidget.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>

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
    LearningPageVideosWidget            _videosWidget;          /** Learning page video content widget */
    LearningPageMiscellaneousWidget     _miscellaneousWidget;   /** Learning page miscellaneous content widget */

    friend class LearningPageWidget;
};