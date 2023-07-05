// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "InfoWidget.h"

#include "util/WidgetOverlayer.h"

#include <QWidget>
#include <QLabel>

namespace hdps::gui
{

/**
 * Info overlay widget class
 *
 * Overlays the parent widget with an info widget (and synchronizes with its geometry) .
 *  
 * @author Thomas Kroes
 */
class InfoOverlayWidget : public InfoWidget
{
public:

    /**
     * Construct with \p parent
     * @param parent Pointer to parent widget
     */
    InfoOverlayWidget(QWidget* parent);

    /**
     * Construct with \parent, \p icon, \p title, \p description, \p backgroundColor and \p textColor
     * @param parent Pointer to parent widget
     * @param icon Icon
     * @param title Title of the overlay
     * @param description Overlay description
     * @param foregroundColor Foreground color
     * @param backgroundColor Background color
     */
    InfoOverlayWidget(QWidget* parent, const QIcon& icon, const QString& title, const QString& description = "", const QColor foregroundColor = Qt::black, const QColor backgroundColor = Qt::lightGray);

    /**
     * Get widget fader
     * @return Reference to widget fader
     */
    util::WidgetFader& getWidgetFader();

private:
    hdps::util::WidgetOverlayer     _widgetOverlayer;      /** Utility for layering on top of the target widget */
};

}
