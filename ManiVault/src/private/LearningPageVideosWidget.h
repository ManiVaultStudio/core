// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QWidget>
#include <QScrollArea>

#include "LearningPageVideosModel.h"
#include "LearningPageVideosFilterModel.h"

#include <widgets/FlowLayout.h>

#include <actions/HorizontalGroupAction.h>

class LearningPageContentWidget;

/**
 * Learning page videos widget class
 *
 * Widget class which lists learning page video content
 *
 * @author Thomas Kroes
 */
class LearningPageVideosWidget : public QWidget
{
protected:

    /**
     * Construct with pointer to \p learningPageContentWidget
     * @param learningPageContentWidget Pointer to owning learning page content widget
     */
    LearningPageVideosWidget(LearningPageContentWidget* learningPageContentWidget);

private:

    /** Update all  custom style elements */
    void updateCustomStyle();

    void updateVideos();

private:
    LearningPageContentWidget*      _learningPageContentWidget;     /** Pointer to owning learning page content widget */
    QVBoxLayout                     _mainLayout;                    /** Main vertical layout */
    mv::gui::HorizontalGroupAction  _settingsAction;                
    QScrollArea                     _videosScrollArea;
    QWidget                         _videosWidget;
    QGridLayout                     _videosLayout;
    LearningPageVideosModel         _model;                         /** Videos model */
    LearningPageVideosFilterModel   _filterModel;                   /** Videos filter model */

    friend class LearningPageContentWidget;
};
