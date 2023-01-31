#pragma once

#include <QObject>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

namespace hdps {

namespace util {

class WidgetFader : public QObject
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     */
    WidgetFader(QObject* parent, QWidget* targetWidget) :
        QObject(parent),
        _targetWidget(targetWidget),
        _opacityEffect(this),
        _opacityAnimation(this)
    {
        _targetWidget->setGraphicsEffect(&_opacityEffect);

        _opacityEffect.setOpacity(0.0);

        _opacityAnimation.setTargetObject(&_opacityEffect);
        _opacityAnimation.setPropertyName("opacity");
        _opacityAnimation.setDuration(150);

        parentWidget()->installEventFilter(this);

        setObjectName("HighlightWidget");

        const auto cssStyle = "\
        QWidget#HighlightWidget { \
            background-color: gray; \
            border-radius: 2px; \
        }";

        setStyleSheet(cssStyle);

        updateSize();
        fadeIn();
    }

    /**
        * Respond to \p target events
        * @param target Object of which an event occurred
        * @param event The event that took place
        */
    bool eventFilter(QObject* target, QEvent* event) override {
        switch (event->type())
        {
        case QEvent::Move:
        case QEvent::Resize:
        {
            if (dynamic_cast<QWidget*>(target) == parentWidget())
                updateSize();

            break;
        }

        default:
            break;
        }

        return QWidget::eventFilter(target, event);
    }

    void updateSize() {
        setFixedSize(parentWidget()->size());
    }

    void fadeIn() {
        _opacityAnimation.setStartValue(_opacityEffect.opacity());
        _opacityAnimation.setEndValue(0.5);
        _opacityAnimation.start();
    }

    void fadeOut() {
        _opacityAnimation.setStartValue(_opacityEffect.opacity());
        _opacityAnimation.setEndValue(0.0);
        _opacityAnimation.start();
    }

private:
    QWidget*                _targetWidget;          /** Target widget to fade */
    QGraphicsOpacityEffect  _opacityEffect;         /** Effect for modulating opacity */
    QPropertyAnimation      _opacityAnimation;      /** Opacity animation */
};

}
}
