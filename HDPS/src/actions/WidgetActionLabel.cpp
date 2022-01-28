#include "WidgetActionLabel.h"
#include "WidgetAction.h"
#include "Application.h"

#include <QDebug>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>

namespace hdps {

namespace gui {

WidgetActionLabel::WidgetActionLabel(WidgetAction* widgetAction, QWidget* parent /*= nullptr*/, Qt::WindowFlags windowFlags /*= Qt::WindowFlags()*/) :
    QLabel("", parent, windowFlags),
    _widgetAction(widgetAction),
    _isHovering(false),
    _resetAction(this, "Reset"),
    _saveDefaultAction(this, "Save default")
{
    setAcceptDrops(true);

    // Set action actions
    _resetAction.setIcon(Application::getIconFont("FontAwesome").getIcon("undo"));
    _saveDefaultAction.setIcon(Application::getIconFont("FontAwesome").getIcon("save"));

    // Set tooltips
    _resetAction.setToolTip("Reset to default value");
    _saveDefaultAction.setToolTip("Save default value to disk");

    // Load/save when triggered
    connect(&_resetAction, &TriggerAction::triggered, _widgetAction, &WidgetAction::reset);
    connect(&_saveDefaultAction, &TriggerAction::triggered, _widgetAction, &WidgetAction::saveDefault);

    const auto update = [this, widgetAction]() -> void {
        setEnabled(widgetAction->isEnabled());
        setText(QString("%1: ").arg(widgetAction->text()));
        setToolTip(widgetAction->text());
        setVisible(widgetAction->isVisible());
    };

    connect(widgetAction, &WidgetAction::changed, this, update);

    update();
}

void WidgetActionLabel::mousePressEvent(QMouseEvent* mouseEvent)
{
    if (!_isHovering)
        return;

    if (_widgetAction->getSettingsPrefix().isEmpty())
        return;

    if (mouseEvent->button() != Qt::LeftButton)
        return;

    _resetAction.setEnabled(_widgetAction->isResettable());

    auto contextMenu = new QMenu();

    contextMenu->addAction(&_resetAction);
    contextMenu->addAction(&_saveDefaultAction);

    // Show the context menu
    contextMenu->exec(cursor().pos());
}

void WidgetActionLabel::enterEvent(QEvent* event)
{
    if (!_widgetAction->hasSettingsPrefix())
        return;

    _isHovering = true;

    setStyleSheet("QLabel { text-decoration: underline; }");
}

void WidgetActionLabel::leaveEvent(QEvent* event)
{
    if (!_widgetAction->hasSettingsPrefix())
        return;

    _isHovering = false;

    setStyleSheet("QLabel { text-decoration: none; }");
}

}
}
