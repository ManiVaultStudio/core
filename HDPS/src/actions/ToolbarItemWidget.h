#pragma once

#include "widgets/FadeableWidget.h"

#include <QTimer>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

namespace hdps {

namespace gui {

class ToolbarWidget;

/**
 * Stateful item class
 *
 * Selectively shows either a standard or collapsed widget of an action
 *
 * @author Thomas Kroes
 */
class ToolbarItemWidget : public QWidget {

public:

    /**
     * Constructor
     * @param toolbarWidget Pointer to toolbar widget
     */
    ToolbarItemWidget(ToolbarWidget* toolbarWidget);

    /**
     * Get state
     * @return State
     */
    std::int32_t getState() const;

    /**
     * Set state
     * @param state State
     */
    void setState(std::int32_t state);

    /**
     * Invoked when the state changed
     * @return previousState Previous state of the item widget
     * @return currentState Current state of the item widget
     */
    virtual void stateChanged(std::int32_t previousState, std::int32_t currentState) = 0;

    /**
     * Get size
     * @return Size of the toolbar item widget
     */
    virtual QSize getSize() const = 0;

    /**
     * Get size
     * @param state State for which to fetch the size
     * @return Size of the toolbar item widget
     */
    virtual QSize getSize(const std::int32_t& state) const = 0;

    /** Show the item */
    void show();

    /** Hide the item */
    void hide();

    /**
     * Set visibility
     * @param visible Visibility
     */
    void setVisibility(bool visible);

protected:
    ToolbarWidget*     _toolbarWidget;     /** Pointer to owning toolbar widget */
    std::int32_t       _state;             /** Toolbar item state */
    QVariantAnimation  _sizeAnimation;     /** Animation to control the size of the widget */
    FadeableWidget     _fadeableWidget;    /** For fading in/out */
};

using SharedToolbarItemWidget = QSharedPointer<ToolbarItemWidget>;

}
}
