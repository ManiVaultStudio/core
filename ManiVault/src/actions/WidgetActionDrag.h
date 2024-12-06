// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QObject>

namespace mv::gui {

class WidgetAction;

/**
 * Widget action drag class
 *
 * Implements dragging behaviour for WidgetAction derived classes.
 *
 * Note: This action is developed for internal use only
 * 
 * @author Thomas Kroes
 */
class CORE_EXPORT WidgetActionDrag final : public QObject
{
    Q_OBJECT

public:

    /**
     * Construct with pointer to \p dragAction
     * @param dragAction Pointer to drag action
     */
    WidgetActionDrag(WidgetAction* dragAction);

    /**
     * Get whether dragging is taking place
     * @return Boolean determining whether dragging is taking place
     */
    bool isDragging() const;

    /** Start the dragging process */
    void start();

private:

    /**
     * Set dragging to \p dragging
     * @param dragging Boolean determining if the action is being dragge
     */
    void setIsDragging(bool dragging);

signals:

    /**
     * Signals that the dragging state changed to \p isDragging
     * @param isDragging Boolean determining if the action is being dragged
     */
    void isDraggingChanged(bool isDragging);

private:
    WidgetAction*   _dragAction;    /** Pointer to drag action */
    bool            _isDragging;    /** Boolean determining whether this action is being dragged to connect to another action */
};

}