// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WidgetFader.h"

#include <QDebug>
#include <QTimer>
#include <QWidget>

#ifdef _DEBUG
    //#define WIDGET_FADER_VERBOSE
#endif

namespace mv::util {

WidgetFader::WidgetFader(QObject* parent, QWidget* targetWidget, float opacity /*= 0.0f*/, float minimumOpacity /*= 0.0f*/, float maximumOpacity /*= 1.0f*/, std::int32_t fadeInDuration /*= 150*/, std::int32_t fadeOutDuration /*= 150*/) :
    QObject(parent),
    _targetWidget(targetWidget),
    _minimumOpacity(minimumOpacity),
    _maximumOpacity(maximumOpacity),
    _fadeInDuration(fadeInDuration),
    _fadeOutDuration(fadeOutDuration),
    _opacityEffect(_targetWidget),
    _opacityAnimation(&_opacityEffect, "opacity", this)
{
    Q_ASSERT(_targetWidget != nullptr);

    _targetWidget->setGraphicsEffect(&_opacityEffect);

    connect(&_opacityAnimation, &QPropertyAnimation::finished, this, [this]() -> void {
        if (_opacityEffect.opacity() == _maximumOpacity)
            emit fadedIn();

        if (_opacityEffect.opacity() == _minimumOpacity)
            emit fadedOut();
    });

    connect(&_opacityAnimation, &QPropertyAnimation::valueChanged, this, [this](const QVariant& value) -> void {
        emit opacityChanged(value.toFloat());
    });

    _opacityEffect.setOpacity(opacity);
}

void WidgetFader::fadeIn(std::int32_t duration /*= -1*/)
{
#ifdef WIDGET_FADER_VERBOSE
    qDebug() << __FUNCTION__ << _targetWidget->objectName() << duration;
#endif

    setOpacity(_maximumOpacity, duration >= 0 ? duration : _fadeInDuration);
}

void WidgetFader::fadeOut(std::int32_t duration /*= -1*/, bool autoHideAfterFade /*= false*/)
{
#ifdef WIDGET_FADER_VERBOSE
    qDebug() << __FUNCTION__ << _targetWidget->objectName() << duration;
#endif

    setOpacity(_minimumOpacity, duration >= 0 ? duration : _fadeOutDuration);

    if (autoHideAfterFade) {
        connect(&_opacityAnimation, &QPropertyAnimation::finished, this, [this]() {
            _targetWidget->hide();
        }, Qt::SingleShotConnection);
    }
}

void WidgetFader::setOpacity(float opacity, std::uint32_t duration /*= 0*/, std::uint32_t delay /*= 0*/)
{
#ifdef WIDGET_FADER_VERBOSE
    //qDebug() << __FUNCTION__;
#endif

    const auto startOpacityAnimation = [this, opacity, duration, delay]() -> void {
        if (duration == 0) {
            _opacityEffect.setOpacity(opacity);
        }
        else {
            _opacityAnimation.stop();

            _opacityAnimation.setDuration(static_cast<int>(duration));
            _opacityAnimation.setStartValue(_opacityEffect.opacity());
            _opacityAnimation.setEndValue(opacity);

            QTimer::singleShot(50, this, &WidgetFader::startAnimation);
        }
    };

    if (delay > 0)
        QTimer::singleShot(delay, startOpacityAnimation);
    else
        startOpacityAnimation();
}

bool WidgetFader::isFadedIn() const
{
    return _opacityEffect.opacity() == _maximumOpacity;
}

bool WidgetFader::isFadedOut() const
{
    return _opacityEffect.opacity() == _minimumOpacity;
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

QGraphicsOpacityEffect& WidgetFader::getOpacityEffect()
{
    return _opacityEffect;
}

void WidgetFader::startAnimation()
{
    _opacityAnimation.start();
}

}
