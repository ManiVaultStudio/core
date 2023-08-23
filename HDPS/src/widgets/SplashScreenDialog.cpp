// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SplashScreenDialog.h"
#include "Application.h"

#include <QApplication>
#include <QScreen>
#include <QTimer>
#include <QPainter>

#include <algorithm>

#ifdef _DEBUG
    #define SPLASH_SCREEN_DIALOG_VERBOSE
#endif

namespace hdps::gui {

SplashScreenDialog::SplashScreenDialog(QWidget* parent, std::int32_t autoHideTimeout /*= DEFAULT_AUTO_HIDE_TIMEOUT*/, std::uint32_t panAmount /*= DEFAULT_PAN_AMOUNT*/, std::uint32_t animationDuration /*= DEFAULT_ANIMATION_DURATION*/) :
    QDialog(parent),
    _opacityAnimation(this, "windowOpacity", this),
    _positionAnimation(this, "pos", this),
    _animationState(AnimationState::Idle),
    _autoHideTimeout(),
    _panAmount(),
    _animationDuration()
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window | Qt::WindowStaysOnTopHint); // | Qt::Popup
    setFixedSize(QSize(640, 480));

    initialize(autoHideTimeout, panAmount, animationDuration);
}

void SplashScreenDialog::initialize(std::int32_t autoHideTimeout /*= DEFAULT_AUTO_HIDE_TIMEOUT*/, std::uint32_t panAmount /*= DEFAULT_PAN_AMOUNT*/, std::uint32_t animationDuration /*= DEFAULT_ANIMATION_DURATION*/)
{
    _autoHideTimeout    = autoHideTimeout;
    _panAmount          = panAmount;
    _animationDuration  = std::min(static_cast<std::int32_t>(animationDuration), _autoHideTimeout / 2);

    _opacityAnimation.setDuration(_animationDuration);
    _positionAnimation.setDuration(_animationDuration);
}

void SplashScreenDialog::open()
{
#ifdef SPLASH_SCREEN_DIALOG_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    fadeIn();

    QDialog::open();

    if (_autoHideTimeout > 0) {
        QTimer::singleShot(_autoHideTimeout - _animationDuration, [this]() -> void {
            accept();
        });
    }
}

void SplashScreenDialog::close()
{
#ifdef SPLASH_SCREEN_DIALOG_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    QDialog::close();
}

void SplashScreenDialog::accept()
{
#ifdef SPLASH_SCREEN_DIALOG_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    if (_animationDuration > 0) {
        connect(&_opacityAnimation, &QPropertyAnimation::finished, this, [this]() -> void {
            QDialog::accept();
        });
    }
    else
        QDialog::accept();
}

void SplashScreenDialog::reject()
{
#ifdef SPLASH_SCREEN_DIALOG_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    if (_animationDuration > 0) {
        connect(&_opacityAnimation, &QPropertyAnimation::finished, this, [this]() -> void {
            QDialog::reject();
        });
    }
    else
        QDialog::reject();
}

void SplashScreenDialog::setBackgroundImage(const QPixmap& backgroundImage)
{
    _backgroundImage = backgroundImage;

    update();
}

void SplashScreenDialog::paintEvent(QPaintEvent* paintEvent)
{
    QPainter painter(this);

    //painter.setBackground(QBrush(_projectSplashScreenAction.getBackgroundColorAction().getColor()));

    auto centerOfWidget = rect().center();
    auto pixmapRectangle = _backgroundImage.rect();

    pixmapRectangle.moveCenter(centerOfWidget - QPoint(200, 0));

    painter.setOpacity(0.5);
    painter.drawPixmap(pixmapRectangle.topLeft(), _backgroundImage);
}

void SplashScreenDialog::fadeIn()
{
    if (_animationDuration <= 0)
        return;

    if (_animationState == AnimationState::FadingIn)
        return;

#ifdef SPLASH_SCREEN_DIALOG_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    _animationState = AnimationState::FadingIn;

    setEnabled(false);

    if (_opacityAnimation.state() == QPropertyAnimation::Running)
        _opacityAnimation.stop();
    
    _opacityAnimation.setEasingCurve(QEasingCurve::InQuad);
    _opacityAnimation.setStartValue(_opacityAnimation.currentValue().toFloat());
    _opacityAnimation.setEndValue(1.0);
    _opacityAnimation.start();

    if (_positionAnimation.state() == QPropertyAnimation::Running)
        _positionAnimation.stop();

    const auto primaryScreenGeometry    = QApplication::primaryScreen()->geometry();
    const auto position                 = primaryScreenGeometry.topLeft() + primaryScreenGeometry.center() - rect().center();

    _positionAnimation.setEasingCurve(QEasingCurve::OutQuad);
    _positionAnimation.setStartValue(position - QPoint(0, _panAmount));
    _positionAnimation.setEndValue(position);
    _positionAnimation.start();
}

void SplashScreenDialog::fadeOut()
{
    if (_animationDuration <= 0)
        return;

    if (_animationState == AnimationState::FadingOut)
        return;

#ifdef SPLASH_SCREEN_DIALOG_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    _animationState = AnimationState::FadingOut;

#ifdef _DEBUG
    qDebug() << __FUNCTION__;
#endif

    if (_opacityAnimation.state() == QPropertyAnimation::Running)
        _opacityAnimation.stop();

    _opacityAnimation.setEasingCurve(QEasingCurve::InQuad);
    _opacityAnimation.setStartValue(_opacityAnimation.currentValue().toFloat());
    _opacityAnimation.setEndValue(0.0);
    _opacityAnimation.start();

    if (_positionAnimation.state() == QPropertyAnimation::Running)
        _positionAnimation.stop();

    const auto primaryScreenGeometry    = QApplication::primaryScreen()->geometry();
    const auto position                 = primaryScreenGeometry.topLeft() + primaryScreenGeometry.center() - rect().center();


    _positionAnimation.setEasingCurve(QEasingCurve::InQuad);
    _positionAnimation.setStartValue(_positionAnimation.currentValue().toPoint());
    _positionAnimation.setEndValue(position + QPoint(0, _panAmount));
    _positionAnimation.start();
}

}
