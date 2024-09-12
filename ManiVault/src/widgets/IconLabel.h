// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QEnterEvent>
#include <QEvent>
#include <QIcon>
#include <QGraphicsOpacityEffect>
#include <QLabel>
#include <QString>

#include <functional>

namespace mv::gui
{

/**
 * Icon label class
 *
 * Icon label with tooltip (obtained with tooltip callback function)
 *  
 * @author Thomas Kroes
 */
class CORE_EXPORT IconLabel : public QLabel
{
public:

    using TooltipCallback = std::function<QString()>;    /** Callback function that is called when a tooltip is required */

public:

    /**
     * Construct from \p icon and \p parent widget
     * @param icon Label icon
     * @param parent Pointer to parent widget
     */
    IconLabel(const QIcon& icon, QWidget* parent = nullptr);

    /**
     * Set the tooltip callback
     * @param tooltipCallback Callback function that is called when a tooltip is required
     */
    void setTooltipCallback(const TooltipCallback& tooltipCallback);

    /**
     * Triggered on mouse hover
     * @param enterEvent Pointer to enter event
     */
    void enterEvent(QEnterEvent* enterEvent) override;

    /**
     * Triggered on mouse leave
     * @param event Pointer to event
     */
    void leaveEvent(QEvent* event) override;

private:

    /** Updates the label opacity depending on whether the mouse is hovering over the label */
    void updateOpacityEffect();

private:
    QGraphicsOpacityEffect  _opacityEffect;         /** Effect for modulating label opacity */
    TooltipCallback         _tooltipCallback;       /** Callback function that is called when a tooltip is required */
};

}
