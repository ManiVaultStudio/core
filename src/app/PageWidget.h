// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "PageHeaderWidget.h"

#include "util/Serializable.h"

#include <QWidget>

/**
 * Page widget class
 *
 * Base widget class which show standard page content (background etc.)
 *
 * @author Thomas Kroes
 */
class PageWidget : public QWidget, public mv::util::Serializable
{
public:

    /**
     * Construct with \p title and pointer to \p parent widget
     * @param title Header title
     * @param parent Pointer to parent widget
     */
    explicit PageWidget(const QString& title, QWidget* parent = nullptr);

    /**
     * Override paint event to draw the logo in the background
     * @param paintEvent Pointer to paint event
     */
    void paintEvent(QPaintEvent* paintEvent) override;

    /**
     * Get page header widget
     * @return Reference to page header widget
     */
    PageHeaderWidget& getPageHeaderWidget();

protected:

    /**
     * Get center content layout
     * @return Vertical content box layout
     */
    QVBoxLayout& getContentLayout();

private:

    /** Update the logo shown in the start page */
    void updateLogo();

    /** Update the title of the start page */
    void updateTitle();

private:
    QHBoxLayout         _layout;                /** Main layout */
    QVBoxLayout         _contentLayout;         /** Layout for the main content */
    QLabel              _titleLabel;            /** Title label */
    PageHeaderWidget    _pageHeaderWidget;      /** Widget which contains the header */
    QPixmap             _backgroundImage;       /** Background image */
};
