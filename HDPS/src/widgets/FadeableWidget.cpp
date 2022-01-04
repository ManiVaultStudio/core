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

void FadeableWidget::setOpacity(float opacity, std::int32_t duration /*= 0*/, std::int32_t delay /*= 0*/, std::function<void()> finished /*= std::function<void()>()*/)
{
    // No need to proceed if the opacity did not change
    if (opacity == _opacityEffect.opacity())
        return;

    // Call callback if one is specified
    const auto done = [finished]() -> void {
        if (finished)
            finished();
    };

    if (duration == 0) {

        // Set opacity after delay or instantly
        if (delay > 0) {
            QTimer::singleShot(delay, [this, opacity, done]() {
                _opacityEffect.setOpacity(opacity);
                done();
            });
        }
        else {
            _opacityEffect.setOpacity(opacity);
            done();
        }
    }
    else {

        // Set opacity animation parameters
        _opacityAnimation.setDuration(duration);
        _opacityAnimation.setEasingCurve(QEasingCurve::InQuad);
        _opacityAnimation.setStartValue(_opacityEffect.opacity());
        _opacityAnimation.setEndValue(opacity);

        // Call callback once finished (and if one is specified)
        connect(&_opacityAnimation, &QPropertyAnimation::finished, done);

        connect(&_opacityAnimation, &QPropertyAnimation::valueChanged, _target, &QWidget::updateGeometry);

        // Start opacity animation after delay or instantly
        if (delay > 0) {
            QTimer::singleShot(delay, [this]() {
                _opacityAnimation.start();
            });
        }
        else
            _opacityAnimation.start();
    }
}

void FadeableWidget::fadeIn(std::int32_t duration, std::int32_t delay /*= 0*/, std::function<void()> finished /*= std::function<void()>()*/)
{
    setOpacity(1.0f, duration, delay, finished);
}

void FadeableWidget::fadeOut(std::int32_t duration, std::int32_t delay /*= 0*/, std::function<void()> finished /*= std::function<void()>()*/)
{
    setOpacity(0.0f, duration, delay, finished);
}

}
}
