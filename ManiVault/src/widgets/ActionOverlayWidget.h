// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "OverlayWidget.h"

namespace mv::gui
{

/**
 * Action overlay widget class
 *
 * Overlays the target widget with the action widget in a chosen location (alignment).
 *
 * Note: This action is developed for internal use only and is in a WIP state
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ActionOverlayWidget : public OverlayWidget
{
public:

    /**
     * Construct with pointer to \p target widget and initial opacity
     * @param target Pointer to target widget (used to synchronize the geometry with, may not be nullptr)
     * @param action Pointer to action (may not be nullptr)
     * @param alignment Alignment of the overlay action in the target widget
     */
    ActionOverlayWidget(QWidget* target, const QPointer<WidgetAction>& action, const Qt::Alignment& alignment = Qt::AlignTop | Qt::AlignLeft);

    /**
     * Set target widget for overlay action to \p targetWidget
     * @param targetWidget Pointer to target widget (may not be nullptr)
     */
    void setTargetWidget(QWidget* targetWidget);

private:
    QPointer<WidgetAction>  _action;        /** Pointer to action (may not be nullptr) */
    QWidget*                _actionWidget;  /** Pointer to action widget (maybe nullptr) */
};

}
