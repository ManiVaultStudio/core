#pragma once

#include <QObject>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

namespace hdps {

namespace util {

/**
 * Widget fader utility class
 *
 * Helper class for fading in/out a target widget.
 *
 * @author Thomas Kroes
 */
class WidgetFader : public QObject
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param targetWidget Pointer to target widget to fade
     * @param opacity Initial opacity [0, 1]
     * @param minimumOpacity Target fade in opacity [0, 1]
     * @param maximumOpacity Target fade out opacity [0, 1]
     * @param animationDuration Duration of the fade in animation
     * @param animationDuration Duration of the fade out animation
     */
    WidgetFader(QObject* parent, QWidget* targetWidget, float opacity = 0.0f, float minimumOpacity = 0.0f, float maximumOpacity = 1.0f, std::int32_t fadeInDuration = 150, std::int32_t fadeOutDuration = 150);

    /** Fade in the \p _targetWidget */
    void fadeIn();

    /** Fade out the \p _targetWidget */
    void fadeOut();

private:
    QWidget*                _targetWidget;          /** Pointer to target widget to fade */
    float                   _minimumOpacity;        /** Target fade out opacity */
    float                   _maximumOpacity;        /** Target fade in opacity */
    std::int32_t            _fadeInDuration;        /** Duration of the fade in animation */
    std::int32_t            _fadeOutDuration;       /** Duration of the fade out animation */
    QGraphicsOpacityEffect  _opacityEffect;         /** Effect for modulating opacity */
    QPropertyAnimation      _opacityAnimation;      /** Opacity animation */
};

}
}
