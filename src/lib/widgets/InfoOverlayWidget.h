// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "InfoWidget.h"

#include "util/WidgetFader.h"
#include "util/WidgetOverlayer.h"

#include <QWidget>
#include <QLabel>

namespace mv::gui
{

/**
 * Info overlay widget class
 *
 * Overlays the parent widget with an info widget (and synchronizes with its geometry) .
 *  
 * @author Thomas Kroes
 */
class CORE_EXPORT InfoOverlayWidget : public InfoWidget
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
     */
    InfoOverlayWidget(QWidget* parent, const QIcon& icon, const QString& title, const QString& description = "");

    /**
     * Get widget fader
     * @return Reference to widget fader
     */
    util::WidgetFader& getWidgetFader();

private:
    mv::util::WidgetOverlayer   _widgetOverlayer;   /** Utility for layering on top of the target widget */
    mv::util::WidgetFader       _widgetFader;       /** Widget fader for animating the widget opacity */
};

}
