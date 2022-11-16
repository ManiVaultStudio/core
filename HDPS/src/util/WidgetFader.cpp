#include "WidgetFader.h"

#include <QDebug>
#include <QWidget>

//#define WIDGET_FADER_VERBOSE

namespace hdps {

namespace util {

WidgetFader::WidgetFader(QObject* parent, QWidget* targetWidget, float opacity /*= 0.0f*/, float minimumOpacity /*= 0.0f*/, float maximumOpacity /*= 1.0f*/, std::int32_t fadeInDuration /*= 150*/, std::int32_t fadeOutDuration /*= 150*/) :
    QObject(parent),
    _targetWidget(targetWidget),
    _minimumOpacity(minimumOpacity),
    _maximumOpacity(maximumOpacity),
    _fadeInDuration(fadeInDuration),
    _fadeOutDuration(fadeOutDuration),
    _opacityEffect(this),
    _opacityAnimation(this)
{
    Q_ASSERT(_targetWidget != nullptr);

    _targetWidget->setGraphicsEffect(&_opacityEffect);

    _opacityEffect.setOpacity(opacity);

    _opacityAnimation.setTargetObject(&_opacityEffect);
    _opacityAnimation.setPropertyName("opacity");
}

void WidgetFader::fadeIn()
{
#ifdef WIDGET_FADER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    if (_opacityAnimation.state() == QPropertyAnimation::Running)
        _opacityAnimation.stop();
    
    _opacityAnimation.setDuration(_fadeInDuration);
    _opacityAnimation.setStartValue(_opacityEffect.opacity());
    _opacityAnimation.setEndValue(_maximumOpacity);
    _opacityAnimation.start();
}

void WidgetFader::fadeOut()
{
#ifdef WIDGET_FADER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    if (_opacityAnimation.state() == QPropertyAnimation::Running)
        _opacityAnimation.stop();

    _opacityAnimation.setDuration(_fadeOutDuration);
    _opacityAnimation.setStartValue(_opacityEffect.opacity());
    _opacityAnimation.setEndValue(_minimumOpacity);
    _opacityAnimation.start();
}

float WidgetFader::getMinimumOpacity() const
{
    return _minimumOpacity;
}

float WidgetFader::getMaximumOpacity() const
{
    return _maximumOpacity;
}

void WidgetFader::setMinimumOpacity(float minimumOpacity)
{
    if (minimumOpacity == _minimumOpacity)
        return;

    _minimumOpacity = minimumOpacity;
}

void WidgetFader::setMaximumOpacity(float maximumOpacity)
{
    if (maximumOpacity == _maximumOpacity)
        return;

    _maximumOpacity = maximumOpacity;
}

std::int32_t WidgetFader::getFadeInDuration() const
{
    return _fadeInDuration;
}

std::int32_t WidgetFader::getFadeOutDuration() const
{
    return _fadeOutDuration;
}

void WidgetFader::setFadeInDuration(std::int32_t fadeInDuration)
{
    if (fadeInDuration == _fadeInDuration)
        return;

    _fadeInDuration = fadeInDuration;
}

void WidgetFader::setFadeOutDuration(std::int32_t fadeOutDuration)
{
    if (fadeOutDuration == _fadeOutDuration)
        return;

    _fadeOutDuration = fadeOutDuration;
}

}

}
