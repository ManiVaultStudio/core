// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/WidgetAction.h"
#include "widgets/OverlayWidget.h"

namespace mv::gui {

class WidgetAction;

/**
 * Widget action badge overlay widget class
 *
 * Overlay widget class for showing a badge over an action widget
 *
 * Note: This action is developed for internal use only
 * 
 * @author Thomas Kroes
 */
class CORE_EXPORT WidgetActionBadgeOverlayWidget final : public OverlayWidget
{
public:

    /**
     * Construct with parent widget
     * @param parent Pointer to parent widget
     * @param action Pointer to widget action to highlight
     */
    WidgetActionBadgeOverlayWidget(QWidget* parent, WidgetAction* action);

    /**
     * Paint event
     * @param paintEvent Pointer to paint event
     */
    void paintEvent(QPaintEvent* paintEvent);

private:
    WidgetAction*   _action;    /** Pointer to owning widget action */
};

}