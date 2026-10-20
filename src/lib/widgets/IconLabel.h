// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "util/StyledIcon.h"

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

    using TooltipCallback = std::function<QString()>;   /** Callback function that is called when a tooltip is required */
    using ClickedCallback = std::function<void()>;      /** Callback function that is called when the icon label is clicked (used by non-QObject derived classes) */

public:

    /**
     * Construct from \p icon and \p parent widget
     * @param icon Icon
     * @param parent Pointer to parent widget
     */
    IconLabel(const QIcon& icon, QWidget* parent = nullptr);

    /**
     * Set the icon to \p icon
     * @param icon Icon to set
     */
    void setIcon(const QIcon& icon);

    /**
     * Get the tooltip callback
     * @return TooltipCallback that is called when the sub-action is hovered
     */
    TooltipCallback getTooltipCallback() const;

    /**
     * Set the tooltip callback to \p tooltipCallback
     * @param tooltipCallback Callback function that is called when a tooltip is required
     */
    void setTooltipCallback(const TooltipCallback& tooltipCallback);

    /**
     * Get the clicked callback
     * @return ClickedCallback that is called when the sub-action is clicked
     */
    ClickedCallback getClickedCallback() const;

    /**
     * Set the clicked callback to \p clickedCallback
     * @param clickedCallback Callback function that is called when the icon label is clicked (used by non-QObject derived classes)
     */
    void setClickedCallback(const ClickedCallback& clickedCallback);

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

    /**
     * Handle events
     * @param event Pointer to event
     * @return True if the event was handled, false otherwise
     */
    bool event(QEvent* event) override;

private:

    /** Updates the label opacity depending on whether the mouse is hovering over the label */
    void updateOpacityEffect();

private:
    QGraphicsOpacityEffect  _opacityEffect;         /** Effect for modulating label opacity */
    TooltipCallback         _tooltipCallback;       /** Callback function that is called when a tooltip is required */
    ClickedCallback         _clickedCallback;       /** Callback function that is called when the icon label is clicked (used by non-QObject derived classes) */
};

}
