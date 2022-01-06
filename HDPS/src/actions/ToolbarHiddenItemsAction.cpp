#include "ToolbarHiddenItemsAction.h"
#include "ToolbarAction.h"
#include "Application.h"

namespace hdps {

namespace gui {

ToolbarHiddenItemsAction::ToolbarHiddenItemsAction(ToolbarAction& toolbarAction) :
    WidgetAction(&toolbarAction),
    _toolbarAction(toolbarAction)
{
    setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("ellipsis-h"));
}

ToolbarAction& ToolbarHiddenItemsAction::getToolbarAction()
{
    return _toolbarAction;
}

ToolbarHiddenItemsAction::Widget::Widget(QWidget* parent, ToolbarHiddenItemsAction* toolbarHiddenItemsAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, toolbarHiddenItemsAction, widgetFlags),
    _hiddenItemsAction(toolbarHiddenItemsAction),
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
