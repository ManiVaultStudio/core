// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "util/WidgetFader.h"
#include "widgets/OverlayWidget.h"

#include "WidgetAction.h"

namespace mv::gui {

/**
 * Widget action highlight class
 *
 * Overlay widget class for highlighting a widget action
 *
 * Note: This action is developed for internal use only
 * 
 * @author Thomas Kroes
 */
class CORE_EXPORT WidgetActionHighlightWidget final : public OverlayWidget
{
public:

    /**
     * Construct with parent widget
     * @param parent Pointer to parent widget
     * @param action Pointer to widget action to highlight
     */
    WidgetActionHighlightWidget(QWidget* parent, WidgetAction* action);

    /**
     * Get the highlight action
     * @return Pointer to highlight action
     */
    WidgetAction* getAction();

    /**
     * Set the highlight action
     * @param action Pointer to highlight action
     */
    void setAction(WidgetAction* action);

private:

    /**
     * Invoked when the action highlighting state changes (update the visual representation of the highlight)
     * @param highlighting Action highlighting state
     */
    void highlightingChanged(const WidgetAction::HighlightOption& highlighting);

private:
    WidgetAction*           _action;        /** Pointer to widget action to highlight */
    mv::util::WidgetFader   _widgetFader;   /** Widget fader for animating the widget opacity */
};

}