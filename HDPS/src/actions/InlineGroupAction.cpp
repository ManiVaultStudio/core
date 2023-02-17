#include "InlineGroupAction.h"

#include <QHBoxLayout>

namespace hdps::gui {

InlineGroupAction::InlineGroupAction(QObject* parent, const QString& title /*= ""*/) :
    WidgetAction(parent)
{
    setText(title);
}

QString InlineGroupAction::getTypeString() const
{
    return "InlineGroup";
}

InlineGroupAction::Widget::Widget(QWidget* parent, InlineGroupAction* inlineGroupAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, inlineGroupAction, widgetFlags),
    _inlineGroupAction(inlineGroupAction)
{
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    for (auto child : inlineGroupAction->children()) {
        auto widgetAction = dynamic_cast<WidgetAction*>(child);

        if (widgetAction == nullptr)
            continue;

        layout->addWidget(widgetAction->createWidget(this), widgetAction->getStretch());
    }

    setLayout(layout);
}

}
