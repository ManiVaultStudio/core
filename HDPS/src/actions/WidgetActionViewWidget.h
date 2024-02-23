// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QWidget>

namespace mv::gui {

class WidgetAction;
class WidgetActionHighlightWidget;

/**
 * Widget action view widget class
 *
 * Base class for widgets that provide a view on an action.
 * 
 * Note: This widget class is developed for internal use (not meant to be used explicitly in third-party plugins)
 * 
 * @author Thomas Kroes
 */
class WidgetActionViewWidget : public QWidget
{
public:

    /** Reserved widget view flags */
    enum WidgetFlag {
        PopupLayout                 = 0x10000,      /** Widget with popup layout */
        NoGroupBoxInPopupLayout     = 0x20000       /** Prevent groupbox in popup layout */
    };

public:

    /**
     * Constructor
     * @param parent Parent widget
     * @param action Pointer to the  action that will be displayed
     * @param widgetFlags Widget flags for the configuration of the widget
     */
    WidgetActionViewWidget(QWidget* parent, WidgetAction* action, std::int32_t widgetFlags = 0);

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

    /**
     * Get widget flags
     * @return Widget flags
     */
    std::int32_t getWidgetFlags() const;

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
    std::int32_t                    _widgetFlags;           /** Widget creation flags */
    WidgetActionHighlightWidget*    _highlightWidget;       /** Pointer to highlight widget */
    std::int32_t                    _cachedHighlighting;    /** Cached highlighting */
};

}
