#include "ToolbarAction.h"

#include <QCoreApplication>

namespace hdps {

namespace gui {

ToolbarAction::ToolbarAction(QObject* parent, const QString& title /*= ""*/) :
    WidgetAction(parent),
    _items()
{
    setText(title);
    setMayReset(true);
    setDefaultWidgetFlags(WidgetFlag::Horizontal);
}

void ToolbarAction::addAction(WidgetAction* action, std::int32_t priority /*= 1*/)
{
    _items << Item(action, priority);

    std::sort(_items.begin(), _items.end());
}

ToolbarAction::HorizontalWidget::HorizontalWidget(QWidget* parent, ToolbarAction* toolbarAction) :
    QWidget(parent),
    _toolbarAction(toolbarAction),
    _resizeTimer(),
    _mainLayout(),
    _toolbarLayout(),
    _toolbarWidget()
{
    //setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);

    // Set resize timer interval and only timeout once
    _resizeTimer.setInterval(RESIZE_TIMER_INTERVAL);
    _resizeTimer.setSingleShot(true);

    // Handle resize timer timeout
    connect(&_resizeTimer, &QTimer::timeout, this, &HorizontalWidget::computeLayout);

    for (auto& item : _toolbarAction->_items) {
        _toolbarLayout.addWidget(item.getAction()->createCollapsedWidget(this));
        _toolbarLayout.addWidget(item.getAction()->createWidget(this));
    }

    // Configure toolbar layout
    _toolbarLayout.setMargin(0);
    _toolbarLayout.setSpacing(0);
    _toolbarLayout.setSizeConstraint(QLayout::SetFixedSize);
    _toolbarLayout.addStretch(1);

    // Apply toolbar layout to toolbar widget
    _toolbarWidget.setLayout(&_toolbarLayout);
    _toolbarWidget.setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    _toolbarWidget.setMinimumSize(0, 0);

    // Add toolbar widget to main layout
    _mainLayout.addWidget(&_toolbarWidget);
    _mainLayout.addStretch(1);

    // Apply main layout
    setLayout(&_mainLayout);

    // Configure main layout
    _mainLayout.setMargin(4);
}

void ToolbarAction::HorizontalWidget::resizeEvent(QResizeEvent* resizeEvent)
{
    // Prevent resize timer timeout
    if (_resizeTimer.isActive())
        _resizeTimer.stop();

    // Start resize timer
    _resizeTimer.start();
}

void ToolbarAction::HorizontalWidget::computeLayout()
{
    qDebug() << "Compute toolbar widget layout";

}

QWidget* ToolbarAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setMargin(0);
    layout->setSpacing(3);

    if (widgetFlags & WidgetFlag::Horizontal)
        layout->addWidget(new ToolbarAction::HorizontalWidget(parent, this));

    widget->setLayout(layout);

    return widget;
}

ToolbarAction::Item::Item(WidgetAction* action, std::int32_t priority) :
    _action(action),
    _priority(priority),
    _widths{0, 0}
{
}

WidgetAction* ToolbarAction::Item::getAction()
{
    return _action;
}

}
}
