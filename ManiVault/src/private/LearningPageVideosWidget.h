// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QWidget>

#include "LearningPageVideosModel.h"
#include "LearningPageVideosFilterModel.h"

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

    /**
     * Open persistent editor for \p rowIndex
     * @param rowIndex Index of the row for which to open the persistent editor
     */
    void openPersistentEditor(int rowIndex);

    /**
     * Close persistent editor for \p rowIndex
     * @param rowIndex Index of the row for which to close the persistent editor
     */
    void closePersistentEditor(int rowIndex);

    /** Update all  custom style elements */

    void updateCustomStyle();
private:
    LearningPageContentWidget*      _learningPageContentWidget;     /** Pointer to owning learning page content widget */
    QVBoxLayout                     _mainLayout;                    /** Main vertical layout */
    LearningPageVideosModel         _model;                         /** Videos model */
    LearningPageVideosFilterModel   _filterModel;                   /** Videos filter model */
    mv::gui::HierarchyWidget        _hierarchyWidget;               /** Widget for displaying the video actions */

    friend class LearningPageContentWidget;
};