#pragma once

#include "ToolbarWidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

namespace hdps {

namespace gui {

class ToolbarAction;

/**
 * Horizontal widget class
 *
 * Horizontal toolbar widget for responsive toolbar action
 *
 * @author Thomas Kroes
 */
class VerticalToolbarWidget : public ToolbarWidget
{
protected:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     * @param toolbarAction Reference to toolbar action
     */
    VerticalToolbarWidget(QWidget* parent, ToolbarAction& toolbarAction);

    /**
     * Computes the layout of all items (decides which items are collapsed/standard)
     * @param ignoreToolbarItemWidget Pointer to toolbar item widget that needs to be ignored (state remains the same)
     */
    void computeLayout(ToolbarItemWidget* ignoreToolbarItemWidget = nullptr) override;

protected:
    QHBoxLayout     _layout;            /** Layout */
    QVBoxLayout     _toolbarLayout;     /** Horizontal toolbar layout for items */
    QWidget         _toolbarWidget;     /** Toolbar widget */

    friend class ToolbarAction;
};

}
}
