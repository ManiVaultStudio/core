// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetActionViewWidget.h"

#include <QSize>
#include <QWidget>

namespace mv::gui {

class WidgetAction;

/**
 * Widget action widget class
 *
 * Base class for widgets that interact with a widget action
 * 
 * Note: This action is developed for internal use only
 * 
 * @author Thomas Kroes
 */
class CORE_EXPORT WidgetActionWidget : public WidgetActionViewWidget
{
public:

    /**
     * Constructor
     * @param parent Parent widget
     * @param action Pointer to the widget action that will be displayed
     * @param widgetFlags Widget flags for the configuration of the widget
     */
    WidgetActionWidget(QWidget* parent, WidgetAction* action, std::int32_t widgetFlags = 0);

public: // Popup widget related

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
	void setPopupLayout(QLayout* popupLayout);
};

}
