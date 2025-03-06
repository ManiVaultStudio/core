// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <models/LearningCenterVideosFilterModel.h>

#include <actions/HorizontalGroupAction.h>

#include <QWidget>
#include <QListView>

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

private:

    /** Update all custom style elements */
    void updateCustomStyle();

private:
    QVBoxLayout                             _mainLayout;            /** Main vertical layout */
    mv::gui::HorizontalGroupAction          _settingsAction;        /** For searching etc. */
    QWidget                                 _videosWidget;          /** Container for the videos list view */
    QListView                               _videosListView;        /** List view which shows videos with styled item delegates */
    mv::LearningCenterVideosFilterModel     _videosFilterModel;     /** Videos filter model */

    friend class LearningPageContentWidget;
};
