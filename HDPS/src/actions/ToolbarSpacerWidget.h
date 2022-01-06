#pragma once

#include "ToolbarItemWidget.h"
#include "ToolbarActionWidget.h"

#include <QHBoxLayout>
#include <QFrame>
#include <QVariantAnimation>

namespace hdps {

namespace gui {

class WidgetAction;

/**
 * Stateful item class
 *
 * Selectively shows either a standard or collapsed widget of an action
 *
 * @author Thomas Kroes
 */
class ToolbarSpacerWidget : public ToolbarItemWidget
{

public:

    /** Describes the spacer types */
    enum State {
        Undefined,      /** No spacer type defined */
        Divider,        /** Divide with a line */
        Spacer,         /** Divide with a small space */
        Hidden          /** Spacer is hidden */
    };

public:

    /**
     * Constructor
     * @param toolbarWidget Pointer to toolbar widget
     */
    ToolbarSpacerWidget(ToolbarWidget* toolbarWidget);

    /**
     * Invoked when the state changed
     * @return previousState Previous state of the item widget
     * @return currentState Current state of the item widget
     */
    void stateChanged(std::int32_t previousState, std::int32_t currentState) override;

    /**
     * Get size
     * @return Size of the toolbar item widget
     */
    QSize getSize() const override;

    /**
     * Get size
     * @param state State for which to fetch the size
     * @return Size of the toolbar item widget
     */
    QSize getSize(const std::int32_t& state) const override;

    /**
     * Get state based on previous- and current action widget
     * @param orientation Orientation of the toolbar widget
     * @param statePrevious State of the previous action widget
     * @param stateCurrent State of the current action widget
     */
    static State getState(const Qt::Orientation& orientation, const ToolbarActionWidget::State& statePrevious, const ToolbarActionWidget::State& stateCurrent);

    /**
     * Get size
     * @param orientation Orientation of the toolbar widget
     * @param statePrevious State of the previous action widget
     * @param stateCurrent State of the current action widget
     */
    static QSize getSize(const Qt::Orientation& orientation, const ToolbarActionWidget::State& statePrevious, const ToolbarActionWidget::State& stateCurrent);

    /**
     * Get size
     * @param orientation Orientation of the toolbar widget
     * @param state State of the spacer widget
     */
    static QSize getSize(const Qt::Orientation& orientation, const State& state);

protected:
    ToolbarWidget*      _toolbarWidget;         /** Pointer to owning toolbar widget */
    QHBoxLayout         _layout;                /** Widget layout */
    QFrame              _verticalLine;          /** Vertical line */
    FadeableWidget      _verticalLineFade;      /** Vertical line fade in/out */
    QVariantAnimation   _sizeAnimation;         /** Animation to control the size of the widget */
};

}
}
