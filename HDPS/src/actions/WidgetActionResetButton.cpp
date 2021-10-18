#include "WidgetActionResetButton.h"
#include "WidgetAction.h"
#include "Application.h"

#include <QDebug>

namespace hdps {

namespace gui {

WidgetActionResetButton::WidgetActionResetButton(WidgetAction* widgetAction, QWidget* parent /*= nullptr*/) :
    QToolButton(parent),
    _widgetAction(widgetAction)
{
    // Basic configuration
    setObjectName("ResetPushButton");
    setIcon(Application::getIconFont("FontAwesome").getIcon("undo"));
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setToolButtonStyle(Qt::ToolButtonIconOnly);
    setToolTip(QString("Reset %1 to default").arg(widgetAction->text()));
    setStyleSheet("QToolButton { border : none; }");

    // Enable/disable the reset tool button based on the widget action
    const auto update = [this, widgetAction]() -> void {
        setEnabled(widgetAction->isEnabled() && widgetAction->isResettable());
    };

    connect(widgetAction, &WidgetAction::changed, this, update);

    // Update the tool button when the action changes
    connect(widgetAction, &WidgetAction::resettableChanged, this, update);

    // Reset the widget action when the tool button is clicked
    connect(this, &QToolButton::clicked, this, [this, widgetAction]() -> void {
        widgetAction->reset();
    });

    // Initial update of the tool button
    update();
}

}
}
