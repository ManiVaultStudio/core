#include "WidgetAction.h"

#include <QDebug>
#include <QMenu>
#include <QEvent>
#include <QPainter>
#include <QLabel>
#include <QGroupBox>

namespace hdps {

namespace gui {

WidgetAction::Widget::Widget(QWidget* parent, WidgetAction* widgetAction, const State& state) :
    QWidget(parent),
    _widgetAction(widgetAction),
    _state(state)
{
    const auto updateAction = [this, widgetAction]() -> void {
        setEnabled(widgetAction->isEnabled());
        setVisible(widgetAction->isVisible());
    };

    connect(widgetAction, &QAction::changed, this, [this, updateAction]() {
        updateAction();
    });

    updateAction();
}

void WidgetAction::Widget::setPopupLayout(QLayout* popupLayout)
{
    auto mainLayout = new QVBoxLayout();

    mainLayout->setMargin(4);

    setLayout(mainLayout);

    auto groupBox = new QGroupBox(_widgetAction->text());

    groupBox->setLayout(popupLayout);

    mainLayout->addWidget(groupBox);
}

WidgetAction::WidgetAction(QObject* parent) :
    QWidgetAction(parent)
{
}

WidgetAction::Label* WidgetAction::createLabelWidget(QWidget* parent)
{
    return new WidgetAction::Label(this, parent);
}

QWidget* WidgetAction::getWidget(QWidget* parent, const Widget::State& state /*= Widget::State::Standard*/)
{
    return new QWidget();
}

WidgetAction::CollapsedWidget::CollapsedWidget(QWidget* parent, WidgetAction* widgetAction) :
    Widget(parent, widgetAction, Widget::State::Collapsed),
    _layout(),
    _toolButton()
{
    _layout.setMargin(0);

    _toolButton.setIcon(widgetAction->icon());
    _toolButton.setToolTip(widgetAction->toolTip());
    _toolButton.addAction(widgetAction);
    _toolButton.setPopupMode(QToolButton::InstantPopup);
    _toolButton.setIconSize(QSize(12, 12));
    _toolButton.setFixedSize(QSize(24, 24));
    _toolButton.setStyleSheet("QToolButton::menu-indicator { image: none; }");

    _layout.addWidget(&_toolButton);

    setLayout(&_layout);
}

WidgetAction::StateWidget::StateWidget(QWidget* parent, WidgetAction* widgetAction, const std::int32_t& priority /*= 0*/, const Widget::State& state /*= Widget::State::Compact*/) :
    QWidget(parent),
    _widgetAction(widgetAction),
    _state(state),
    _priority(priority),
    _standardWidget(widgetAction->createWidget(this)),
    _compactWidget(widgetAction->createCollapsedWidget(this))
{
    auto layout = new QHBoxLayout();

    layout->setMargin(0);
    layout->addWidget(_standardWidget);
    layout->addWidget(_compactWidget);

    setState(state);
    setLayout(layout);
}

WidgetAction::Widget::State WidgetAction::StateWidget::getState() const
{
    return _state;
}

void WidgetAction::StateWidget::setState(const Widget::State& state)
{
    _standardWidget->setVisible(state == Widget::State::Standard);
    _compactWidget->setVisible(state == Widget::State::Collapsed);
}

std::int32_t WidgetAction::StateWidget::getPriority() const
{
    return _priority;
}

void WidgetAction::StateWidget::setPriority(const std::int32_t& priority)
{
    _priority = priority;
}

QSize WidgetAction::StateWidget::getSizeHint(const Widget::State& state) const
{
    switch (state)
    {
        case Widget::State::Standard:
            return _standardWidget->sizeHint();

        case Widget::State::Collapsed:
            return _compactWidget->sizeHint();

        default:
            break;
    }

    return QSize();
}

void WidgetAction::ToolButton::paintEvent(QPaintEvent* paintEvent)
{
    QToolButton::paintEvent(paintEvent);

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    const auto margin = 5.0f;

    QPointF center;

    center.setY(height() - margin);
    center.setX(width() - margin);

    painter.setPen(QPen(QBrush(isEnabled() ? Qt::black : Qt::gray), 2.5, Qt::SolidLine, Qt::RoundCap));
    painter.setBrush(Qt::NoBrush);
    painter.drawPoint(center);
}



WidgetAction::Label::Label(WidgetAction* widgetAction, QWidget* parent /*= nullptr*/, Qt::WindowFlags windowFlags /*= Qt::WindowFlags()*/) :
    QLabel(widgetAction->text(), parent, windowFlags)
{
    connect(widgetAction, &WidgetAction::changed, this, [this]() {
        setEnabled(isEnabled());
        setText(text());
        setToolTip(toolTip());
    });
}

}
}