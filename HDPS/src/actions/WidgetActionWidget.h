// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetActionViewWidget.h"

namespace hdps::gui {

class WidgetAction;
class OverlayWidget;

/**
 * Widget action widget class
 *
 * Base class for widgets that interact with a widget action
 * 
 * @author Thomas Kroes
 */
class WidgetActionWidget : public WidgetActionViewWidget
{
public:

    /** Reserved widget settings */
    enum WidgetFlag {
        PopupLayout = 0x10000,      /** Widget with popup layout */
    };

public:

    /**
     * Constructor
     * @param parent Parent widget
     * @param action Pointer to the widget action that will be displayed
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    WidgetActionWidget(QWidget* parent, WidgetAction* action, const std::int32_t& widgetFlags = 0);

public: // Popup widget related
    
    /**
     * Get whether the widget is in a popup state
     * @return Boolean determining whether the widget is in a popup state
     */
    virtual bool isPopup() const final;

    /**
     * Override the size hint to account for popups
     * @return Size hint
     */
    QSize sizeHint() const override;

    /**
     * Override to allow for popup layouts
     * @param layout Pointer to layout
     */
    void setLayout(QLayout* layout);

private:

    /**
     * Set popup layout (layout contains groupbox etc.)
     * @param popupLayout Pointer to the popup layout
     */
    virtual void setPopupLayout(QLayout* popupLayout) final;

protected:
    std::int32_t        _widgetFlags;       /** Widget creation flags */
};

}
