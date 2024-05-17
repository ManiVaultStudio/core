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

    /**
     * Invoked when the widget is shown
     * @param showEvent Pointer to show event
     */
    void showEvent(QShowEvent* showEvent);

private:

    /** Update all custom style elements */
    void updateCustomStyle();

private:
    QVBoxLayout                     _mainLayout;            /** Main vertical layout */
    mv::gui::HorizontalGroupAction  _settingsAction;        /** For searching etc. */
    QScrollArea                     _videosScrollArea;      /** Videos scroll area */
    QWidget                         _videosWidget;          /** Container for the videos list view */
    QListView                       _videosListView;        /** List view which shows videos with styled item delegates */
    LearningPageVideosModel         _model;                 /** Videos model */
    LearningPageVideosFilterModel   _filterModel;           /** Videos filter model */

    friend class LearningPageContentWidget;
};
