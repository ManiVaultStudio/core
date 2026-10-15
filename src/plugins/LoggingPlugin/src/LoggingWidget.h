// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <models/LoggingModel.h>
#include <models/LoggingFilterModel.h>

#include <widgets/HierarchyWidget.h>
#include <actions/TriggerAction.h>

#include <QWidget>

/**
 * Logging widget class
 *
 * Widget class for inspecting real-time logging information
 *
 * @author Thomas Kroes
 */
class LoggingWidget : public QWidget
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     */
    LoggingWidget(QWidget* parent);

    /**
     * Get preferred size of the widget
     * @return Preferred size of the widget
     */
    QSize sizeHint() const override;

private:
    mv::LoggingModel            _model;                     /** Logging model for keeping track of log records */
    mv::LoggingFilterModel      _filterModel;               /** Filter model for logging (source is the logging model) */
    mv::gui::HierarchyWidget    _hierarchyWidget;           /** Widget for displaying hierarchy */
    mv::gui::TriggerAction      _findLogFileAction;         /** Action for finding the log file */
};
