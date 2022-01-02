#include "FadeableWidget.h"

#include <QTimer>

namespace hdps {

namespace gui {

FadeableWidget::FadeableWidget(QWidget* parent, QWidget* target) :
    QWidget(parent),
    _target(target),
    _layout(),
    _opacityEffect(),
    _opacityAnimation(&_opacityEffect, "opacity")
{
    Q_ASSERT(parent != nullptr);
    Q_ASSERT(_target != nullptr);

    _target->setObjectName("TargetWidget");

    _layout.setMargin(0);
    _layout.setSizeConstraint(QLayout::SetFixedSize);
    _layout.addWidget(target);

    setLayout(&_layout);

    _target->setGraphicsEffect(&_opacityEffect);
}

void FadeableWidget::setOpacity(float opacity)
{
    _opacityEffect.setOpacity(opacity);
}

void FadeableWidget::fadeIn(std::int32_t duration, std::int32_t delay /*= 0*/, std::function<void()> finished /*= std::function<void()>()*/)
{
    _opacityAnimation.setDuration(duration);
    _opacityAnimation.setEasingCurve(QEasingCurve::InQuad);
    _opacityAnimation.setStartValue(0.0f);
    _opacityAnimation.setEndValue(1.0f);

    if (finished)
        connect(&_opacityAnimation, &QPropertyAnimation::finished, finished);

    connect(&_opacityAnimation, &QPropertyAnimation::valueChanged, _target, &QWidget::updateGeometry);

    QTimer::singleShot(delay, [this]() {
        _opacityAnimation.start();
    });
}

void FadeableWidget::fadeOut(std::int32_t duration, std::int32_t delay /*= 0*/, std::function<void()> finished /*= std::function<void()>()*/)
{
    _opacityAnimation.setDuration(duration);
    _opacityAnimation.setEasingCurve(QEasingCurve::OutQuad);
    _opacityAnimation.setStartValue(1.0f);
    _opacityAnimation.setEndValue(0.0f);

    if (finished)
        connect(&_opacityAnimation, &QPropertyAnimation::finished, finished);

    connect(&_opacityAnimation, &QPropertyAnimation::valueChanged, _target, &QWidget::updateGeometry);

    QTimer::singleShot(delay, [this]() {
        _opacityAnimation.start();
    });
}

}
}
