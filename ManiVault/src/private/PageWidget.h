// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "PageHeaderWidget.h"

#include <QWidget>

/**
 * Page widget class
 *
 * Base widget class which show standard page content (background etc.)
 *
 * @author Thomas Kroes
 */
class PageWidget : public QWidget
{
public:

    /**
     * Construct with pointer to \p parent widget
     * @param parent Pointer to parent widget
     */
    PageWidget(QWidget* parent = nullptr);

    /**
     * Override paint event to draw the logo in the background
     * @param paintEvent Pointer to paint event
     */
    void paintEvent(QPaintEvent* paintEvent);

    /**
     * Set the widget background color role
     * @param widget Pointer to widget to apply the background color to
     * @param colorRole Background color role
     */
    static void setWidgetBackgroundColorRole(QWidget* widget, const QPalette::ColorRole& colorRole);

protected:

    /**
     * Get center content layout
     * @return Vertical content box layout
     */
    QVBoxLayout& getContentLayout();

private:
    QHBoxLayout         _layout;                /** Main layout */
    QVBoxLayout         _contentLayout;         /** Layout for the main content */
    PageHeaderWidget    _pageHeaderWidget;      /** Widget which contains the header */
    QPixmap             _backgroundImage;       /** Background image */
};
