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
    setObjectName("ResetPushButton");
    setIcon(Application::getIconFont("FontAwesome").getIcon("undo"));
    setIconSize(QSize(12, 12));
    setToolButtonStyle(Qt::ToolButtonIconOnly);
    setToolTip(QString("Reset %1 to default").arg(widgetAction->text()));
    setStyleSheet("QToolButton { border : none; }");

    const auto update = [this, widgetAction]() -> void {
        setEnabled(widgetAction->isEnabled() && widgetAction->isResettable());
    };

    connect(widgetAction, &WidgetAction::changed, this, update);
    connect(widgetAction, &WidgetAction::resettableChanged, this, update);

    connect(this, &QToolButton::clicked, this, [this, widgetAction]() -> void {
        widgetAction->reset();
    });

    update();
}

}
}
