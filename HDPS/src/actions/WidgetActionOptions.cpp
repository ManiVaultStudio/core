#include "WidgetActionOptions.h"
#include "WidgetAction.h"
#include "Application.h"

#include <QDebug>
#include <QLabel>
#include <QMouseEvent>
#include <QMenu>

namespace hdps {

namespace gui {

WidgetActionOptions::WidgetActionOptions(WidgetAction* widgetAction, QLabel* label) :
    QObject(nullptr),
    _widgetAction(widgetAction),
    _label(label),
    _loadDefaultAction(this, "Load default"),
    _saveDefaultAction(this, "Save default"),
    _loadFactoryDefaultAction(this, "Load factory default")
{
    Q_ASSERT(_label != nullptr);

    // Set action icons
    _loadDefaultAction.setIcon(Application::getIconFont("FontAwesome").getIcon("undo"));
    _saveDefaultAction.setIcon(Application::getIconFont("FontAwesome").getIcon("save"));
    _loadFactoryDefaultAction.setIcon(Application::getIconFont("FontAwesome").getIcon("industry"));

    // Set tooltips
    _loadDefaultAction.setToolTip("Reset to default value");
    _saveDefaultAction.setToolTip("Save default value to disk");
    _loadFactoryDefaultAction.setToolTip("Load factory default value");

    // Load when triggered
    connect(&_loadDefaultAction, &TriggerAction::triggered, _widgetAction, [this]() {
        _widgetAction->loadDefault();
    });

    // Save default when triggered
    connect(&_saveDefaultAction, &TriggerAction::triggered, _widgetAction, [this]() {
        _widgetAction->saveDefault(true);
    });

    // Load factory default when triggered
    connect(&_loadFactoryDefaultAction, &TriggerAction::triggered, _widgetAction, &WidgetAction::reset);

    // Install event filter for intercepting label events
    _label->installEventFilter(this);
}

bool WidgetActionOptions::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        // Mouse button press event
        case QEvent::MouseButtonPress:
        {
            auto mouseButtonPress = static_cast<QMouseEvent*>(event);

            if (mouseButtonPress->button() != Qt::LeftButton)
                break;

            _loadDefaultAction.setEnabled(_widgetAction->isResettable());
            _saveDefaultAction.setEnabled(_widgetAction->canSaveDefault());
            _loadFactoryDefaultAction.setEnabled(_widgetAction->isFactoryResettable());

            // Create main context menu
            auto contextMenu = new QMenu();

            // Add load/save default actions
            contextMenu->addAction(&_loadDefaultAction);
            contextMenu->addAction(&_saveDefaultAction);

            contextMenu->addSeparator();

            // Create context menu for additional options
            auto optionsMenu = new QMenu("Options");

            // Configure the context menu and add reset to factory default action
            //optionsMenu->setEnabled(_widgetAction->isFactoryResettable());
            optionsMenu->setIcon(Application::getIconFont("FontAwesome").getIcon("cog"));
            optionsMenu->addAction(&_loadFactoryDefaultAction);

            // Add the options menu to the main context menu
            contextMenu->addMenu(optionsMenu);

            // Show the context menu
            contextMenu->exec(_label->cursor().pos());

            break;
        }

        // Mouse enter event
        case QEvent::Enter:
        {
            _label->setStyleSheet("QLabel { text-decoration: underline; }");
            break;
        }

        // Mouse leave event
        case QEvent::Leave:
        {
            _label->setStyleSheet("QLabel { text-decoration: none; }");
            break;
        }

        default:
            break;
    }

    return QObject::eventFilter(target, event);
}

}
}
