#include "ToolbarAction.h"
#include "HorizontalToolbarWidget.h"
#include "VerticalToolbarWidget.h"
#include "Application.h"

#include <QCoreApplication>
#include <QResizeEvent>

#include <cmath>

namespace hdps {

namespace gui {

ToolbarAction::ToolbarAction(QObject* parent, const QString& title /*= ""*/) :
    WidgetAction(parent),
    _items(),
    _enableAnimation(true),
    _hiddenItemsAction(*this)
{
    setText(title);
    setMayReset(true);
    setDefaultWidgetFlags(WidgetFlag::Horizontal);
}

void ToolbarAction::addAction(WidgetAction* action, std::int32_t priority /*= 1*/)
{
    _items << Item(action, priority);
}

bool ToolbarAction::getEnableAnimation() const
{
    return _enableAnimation;
}

void ToolbarAction::setEnableAnimation(bool enableAnimation)
{
    _enableAnimation = enableAnimation;
}

ToolbarAction::HiddenItemsAction& ToolbarAction::getHiddenItemsAction()
{
    return _hiddenItemsAction;
}

QWidget* ToolbarAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setMargin(0);

    if (widgetFlags & WidgetFlag::Horizontal) {
        widget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::MinimumExpanding);
        layout->addWidget(new HorizontalToolbarWidget(parent, *this));
    }

    if (widgetFlags & WidgetFlag::Vertical) {
        widget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Ignored);

        layout->setAlignment(Qt::AlignTop);
        layout->addWidget(new VerticalToolbarWidget(parent, *this));
    }

    widget->setLayout(layout);

    return widget;
}

ToolbarAction::Item::Item(WidgetAction* action, std::int32_t priority) :
    _action(action),
    _priority(priority)
{
}

WidgetAction* ToolbarAction::Item::getAction()
{
    return _action;
}

std::int32_t ToolbarAction::Item::getPriority() const
{
    return _priority;
}

ToolbarAction::HiddenItemsAction::HiddenItemsAction(ToolbarAction& responsiveToolbarAction) :
    WidgetAction(&responsiveToolbarAction),
    _responsiveToolbarAction(responsiveToolbarAction)
{
    setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("ellipsis-h"));
}

ToolbarAction& ToolbarAction::HiddenItemsAction::getResponsiveToolbarAction()
{
    return _responsiveToolbarAction;
}

ToolbarAction::HiddenItemsAction::Widget::Widget(QWidget* parent, HiddenItemsAction* hiddenItemsAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, hiddenItemsAction, widgetFlags),
    _hiddenItemsAction(hiddenItemsAction),
    _layout()
{
    if (widgetFlags & WidgetActionWidget::PopupLayout) {
        setPopupLayout(&_layout);
    }
    else {
        _layout.setMargin(0);
        setLayout(&_layout);
    }
}

}
}
