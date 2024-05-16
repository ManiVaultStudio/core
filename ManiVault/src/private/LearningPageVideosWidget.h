// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QWidget>
#include <QScrollArea>
#include <QListView>

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
     * Construct with pointer to \p parent widget
     * @param parent pointer to parent widget
     */
    LearningPageVideosWidget(QWidget* parent = nullptr);

    void showEvent(QShowEvent* showEvent);

private:

    /** Update all  custom style elements */
    void updateCustomStyle();

    void updateVideos();

private:
    QVBoxLayout                     _mainLayout;                    /** Main vertical layout */
    mv::gui::HorizontalGroupAction  _settingsAction;                
    QScrollArea                     _videosScrollArea;
    QWidget                         _videosWidget;
    QListView                       _videosListView;
    LearningPageVideosModel         _model;                         /** Videos model */
    LearningPageVideosFilterModel   _filterModel;                   /** Videos filter model */

    friend class LearningPageContentWidget;
};
