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
    _loadDefaultAction(this, "Load default"),
    _saveDefaultAction(this, "Save default")
{
    setAcceptDrops(true);

    // Set action actions
    _loadDefaultAction.setIcon(Application::getIconFont("FontAwesome").getIcon("file-import"));
    _saveDefaultAction.setIcon(Application::getIconFont("FontAwesome").getIcon("file-export"));

    // Set tooltips
    _loadDefaultAction.setToolTip("Load default value from disk");
    _saveDefaultAction.setToolTip("Save default value to disk");

    // Load/save when triggered
    connect(&_loadDefaultAction, &TriggerAction::triggered, _widgetAction, &WidgetAction::loadDefault);
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
    if (_widgetAction->getSettingsPrefix().isEmpty())
        return;

    if (mouseEvent->button() != Qt::LeftButton)
        return;

    auto contextMenu = new QMenu();

    contextMenu->addAction(&_loadDefaultAction);
    contextMenu->addAction(&_saveDefaultAction);

    // Show the context menu
    contextMenu->exec(cursor().pos());
}

void WidgetActionLabel::enterEvent(QEvent* event)
{
    if (!_widgetAction->hasSettingsPrefix())
        return;

    setStyleSheet("QLabel { text-decoration: underline; }");
}

void WidgetActionLabel::leaveEvent(QEvent* event)
{
    if (!_widgetAction->hasSettingsPrefix())
        return;

    setStyleSheet("QLabel { text-decoration: none; }");
}

}
}
