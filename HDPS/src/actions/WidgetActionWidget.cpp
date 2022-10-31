#include "WidgetActionWidget.h"
#include "WidgetAction.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QStyleOption>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QLabel>

namespace hdps {

namespace gui {

class HighlightWidget : public QWidget
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     */
    HighlightWidget(QWidget* parent) :
        QWidget(parent),
        _opacityEffect(this),
        _opacityAnimation(this)
    {
        setAutoFillBackground(true);
        setAttribute(Qt::WA_TransparentForMouseEvents);
        setGraphicsEffect(&_opacityEffect);
        show();
        
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
    QGraphicsOpacityEffect  _opacityEffect;     /** Effect for modulating opacity */
    QPropertyAnimation      _opacityAnimation;  /** Opacity animation */
};

WidgetActionWidget::WidgetActionWidget(QWidget* parent, WidgetAction* widgetAction, const std::int32_t& widgetFlags /*= 0*/) :
    QWidget(parent),
    _widgetAction(widgetAction),
    _widgetFlags(widgetFlags),
    _highlightWidget(nullptr)
{
    setWidgetAction(widgetAction);
}

WidgetAction* WidgetActionWidget::getWidgetAction()
{
    return _widgetAction;
}

void WidgetActionWidget::setWidgetAction(WidgetAction* widgetAction)
{
    if (_widgetAction != nullptr) {
        disconnect(_widgetAction, &WidgetAction::changed, this, nullptr);
        disconnect(_widgetAction, &WidgetAction::highlightedChanged, this, nullptr);
    }

    _widgetAction = widgetAction;

    if (_widgetAction == nullptr)
        return;

    const auto update = [this]() -> void {
        setEnabled(_widgetAction->isEnabled());
        setToolTip(_widgetAction->toolTip());
        setVisible(_widgetAction->isVisible());
    };

    connect(_widgetAction, &WidgetAction::changed, this, update);

    update();

    const auto updateHighlighted = [this]() -> void {
        if (_widgetAction->isHighlighted()) {
            if (_highlightWidget == nullptr)
                _highlightWidget = new HighlightWidget(this);

            _highlightWidget->fadeIn();
        }
        else {
            if (_highlightWidget)
                _highlightWidget->fadeOut();
        }
    };

    connect(_widgetAction, &WidgetAction::highlightedChanged, this, updateHighlighted);

    updateHighlighted();
}

void WidgetActionWidget::setPopupLayout(QLayout* popupLayout)
{
    auto mainLayout = new QVBoxLayout();

    mainLayout->setContentsMargins(4, 4, 4, 4);

    setLayout(mainLayout);

    auto groupBox = new QGroupBox(_widgetAction->text());

    groupBox->setLayout(popupLayout);
    groupBox->setCheckable(_widgetAction->isCheckable());

    mainLayout->addWidget(groupBox);

    const auto update = [this, groupBox]() -> void {
        QSignalBlocker blocker(_widgetAction);

        groupBox->setTitle(_widgetAction->text());
        groupBox->setToolTip(_widgetAction->text());
        groupBox->setChecked(_widgetAction->isChecked());
    };

    connect(groupBox, &QGroupBox::toggled, this, [this](bool toggled) {
        _widgetAction->setChecked(toggled);
    });

    connect(_widgetAction, &WidgetAction::changed, this, [update]() {
        update();
    });

    update();
}

}
}
