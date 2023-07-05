// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "StartPageHeaderWidget.h"
#include "StartPageContentWidget.h"

#include <QWidget>

/**
 * Start page widget class
 *
 * Widget class which show various project-related operations.
 *
 * @author Thomas Kroes
 */
class StartPageWidget : public QWidget
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     */
    StartPageWidget(QWidget* parent = nullptr);

    /**
     * Override paint event to draw the logo in the background
     * @param paintEvent Pointer to paint event
     */
    void paintEvent(QPaintEvent* paintEvent);

    /**
     * Override show event to update start page actions
     * @param showEvent Pointer to show event which occurred
     */
    void showEvent(QShowEvent* showEvent);

    /**
     * Set the widget background color role
     * @param widget Pointer to widget to apply the background color to
     * @param colorRole Background color role
     */
    static void setWidgetBackgroundColorRole(QWidget* widget, const QPalette::ColorRole& colorRole);

protected:
    QHBoxLayout             _layout;                    /** Main layout */
    QVBoxLayout             _centerColumnLayout;        /** Layout for the center column */
    StartPageHeaderWidget   _startPageHeaderWidget;     /** Widget which contains the header */
    StartPageContentWidget  _startPageContentWidget;    /** Widget which contains all the actions */
    QPixmap                 _backgroundImage;           /** Background image */
};
