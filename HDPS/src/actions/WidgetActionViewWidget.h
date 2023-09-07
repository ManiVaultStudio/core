// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QWidget>

namespace hdps::gui {

class WidgetAction;
class WidgetActionHighlightWidget;

/**
 * Widget action view widget class
 *
 * Base class for widgets that provide a view on an action.
 * 
 * @author Thomas Kroes
 */
class WidgetActionViewWidget : public QWidget
{
public:

    /**
     * Constructor
     * @param parent Parent widget
     * @param action Pointer to the  action that will be displayed
     */
    WidgetActionViewWidget(QWidget* parent, WidgetAction* action);

    /**
     * Get the source action
     * @return Pointer to source action
     */
    virtual WidgetAction* getAction() final;

    /**
     * Set the source action
     * @param widgetAction Pointer to source action
     */
    virtual void setAction(WidgetAction* widgetAction);

protected: // Drag-and-drop events

    /**
     * Invoked when a \p dragEnterEvent occurs
     * @param dragEnterEvent Pointer to drag enter event
     */
    void dragEnterEvent(QDragEnterEvent* dragEnterEvent) override;

    /**
     * Invoked when a \p dragLeaveEvent occurs
     * @param dragLeaveEvent Pointer to drag leave event
     */
    void dragLeaveEvent(QDragLeaveEvent* dragLeaveEvent) override;

    /**
     * Invoked when a \p dropEvent occurs
     * @param dropEvent Pointer to drop event
     */
    void dropEvent(QDropEvent* dropEvent) override;

private:
    WidgetAction*                   _action;                /** Pointer to action that will be displayed */
    WidgetActionHighlightWidget*    _highlightWidget;       /** Pointer to highlight widget */
    std::int32_t                    _cachedHighlighting;    /** Cached highlighting */
};

}
