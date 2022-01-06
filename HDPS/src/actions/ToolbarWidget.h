#pragma once

#include "ToolbarItemWidget.h"
#include "ToolbarActionWidget.h"
#include "ToolbarHiddenItemsAction.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>

namespace hdps {

namespace gui {

class ToolbarAction;

/**
 * Responsive toolbar widget class
 *
 * Responsive toolbar widget base class for responsive toolbar action
 *
 * @author Thomas Kroes
 */
class ToolbarWidget : public QWidget
{
protected:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     * @param toolbarAction Reference to toolbar action
     * @param orientation Orientation of the toolbar (horizontal/vertical)
     */
    ToolbarWidget(QWidget* parent, ToolbarAction& toolbarAction, const Qt::Orientation& orientation);

    /**
     * Invoked when the widget is resized
     * @param resizeEvent Pointer to resize event
     */
    void resizeEvent(QResizeEvent* resizeEvent) override;

public:

    /**
     * Get whether animations are enabled or not
     * @return Whether animations are enabled or not
     */
    bool getEnableAnimation() const;

    /**
     * Computes the layout of all items (decides which items are collapsed/standard)
     * @param ignoreToolbarItemWidget Pointer to toolbar item widget that needs to be ignored (state remains the same)
     */
    virtual void computeLayout(ToolbarItemWidget* ignoreToolbarItemWidget = nullptr) = 0;

    /**
     * Get toolbar item widgets
     * @return Vector of shared pointers to toolbar item widgets
     */
    QVector<SharedToolbarItemWidget> getToolbarItemWidgets();

    /**
     * Get toolbar action widgets
     * @return Vector of pointers to toolbar action widgets
     */
    QVector<ToolbarActionWidget*> getToolbarActionWidgets();

protected:
    Qt::Orientation                     _orientation;                   /** Orientation of the toolbar (horizontal/vertical) */
    ToolbarAction&                      _toolbarAction;                 /** Reference to toolbar action */
    QVector<SharedToolbarItemWidget>    _toolbarItemWidgets;            /** All toolbar item widgets (action widgets + spacers) */
    QTimer                              _resizeTimer;                   /** Timer which prevents unnecessary resize handler calls */
    ToolbarHiddenItemsAction            _toolbarHiddenItemsAction;      /** Toolbar hidden items action */

public:
    static constexpr std::int32_t RESIZE_TIMER_INTERVAL = 50;       /** Default resize timer interval */
    static constexpr std::int32_t ANIMATION_DURATION    = 300;      /** Animation duration */
};

}
}
