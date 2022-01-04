#pragma once

#include <QWidget>
#include <QHBoxLayout>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

class QWidget;

namespace hdps {

namespace gui {

/**
 * Fadeable widget class
 *
 * Adds fade in/out capabilities to a target widget
 *
 * @author Thomas Kroes
 */
class FadeableWidget : public QWidget {
public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     * @param target Pointer to target widget to apply the fade in/out to
     */
    FadeableWidget(QWidget* parent, QWidget* target);

    /**
     * Change the widget opacity
     * @param opacity Widget opacity
     * @param duration Duration of the animation in milliseconds
     * @param delay Delay in milliseconds
     * @param finished Function to call when the fade process has completed
     */
    void setOpacity(float opacity, std::int32_t duration = 0, std::int32_t delay = 0, std::function<void()> finished = std::function<void()>());

    /**
     * Fade in the widget
     * @param duration Duration of the animation in milliseconds
     * @param delay Delay in milliseconds
     * @param finished Function to call when the fade process has completed
     */
    void fadeIn(std::int32_t duration, std::int32_t delay = 0, std::function<void()> finished = std::function<void()>());

    /**
     * Fade out the widget
     * @param duration Duration of the animation in milliseconds
     * @param delay Delay in milliseconds
     * @param finished Function to call when the fade process has completed
     */
    void fadeOut(std::int32_t duration, std::int32_t delay = 0, std::function<void()> finished = std::function<void()>());

protected:
    QWidget*                _target;                /** Target widget to apply the fade in/out to */
    QHBoxLayout             _layout;                /** Main layout */
    QGraphicsOpacityEffect  _opacityEffect;         /** Effect for controlling widget opacity */
    QPropertyAnimation      _opacityAnimation;      /** Effect to control the widget opacity over time */
};

}
}
