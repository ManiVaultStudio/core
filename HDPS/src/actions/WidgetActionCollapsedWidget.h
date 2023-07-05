// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetActionViewWidget.h"

#include <QHBoxLayout>
#include <QToolButton>

namespace hdps::gui {

class WidgetAction;

/**
 * Widget action collapsed widget class
 * 
 * Displays a tool button that shows the widget in a popup
 *
 * @author Thomas Kroes
 */
class WidgetActionCollapsedWidget : public WidgetActionViewWidget
{
protected:

    /** Tool button class with custom paint behavior */
    class ToolButton : public QToolButton {
    public:

        /**
         * Paint event
         * @param paintEvent Pointer to paint event
         */
        void paintEvent(QPaintEvent* paintEvent);
    };

public:

    /**
     * Constructor
     * @param parent Parent widget
     * @param action Pointer to the widget action that will be displayed in a popup
     */
    WidgetActionCollapsedWidget(QWidget* parent, WidgetAction* action);

    /**
     * Set the source action
     * @param widgetAction Pointer to source action
     */
    void setAction(WidgetAction* action) override;

    /** Get reference to the tool button */
    ToolButton& getToolButton() { return _toolButton; }

private:
    QHBoxLayout     _layout;            /** Layout */
    ToolButton      _toolButton;        /** Tool button for the popup */

    friend class WidgetAction;
};

}
