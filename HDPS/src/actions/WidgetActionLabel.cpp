#include "WidgetActionLabel.h"
#include "WidgetAction.h"
#include "Application.h"

#include <QDebug>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include <QVBoxLayout>

namespace hdps {

namespace gui {

WidgetActionLabel::WidgetActionLabel(WidgetAction* widgetAction, QWidget* parent /*= nullptr*/, Qt::WindowFlags windowFlags /*= Qt::WindowFlags()*/) :
    QWidget(parent, windowFlags),
    _widgetAction(widgetAction),
    _label(),
    _isHovering(false),
    _loadDefaultAction(this, "Load default"),
    _saveDefaultAction(this, "Save default"),
    _loadFactoryDefaultAction(this, "Load factory default")
{
    auto layout = new QVBoxLayout();

    layout->setAlignment(Qt::AlignRight | Qt::AlignCenter);

    layout->setMargin(0);
    layout->addStretch(1);
    layout->addWidget(&_label);
    layout->addStretch(1);

    setLayout(layout);

    _label.setAlignment(Qt::AlignRight);

    // Set action actions
    _loadDefaultAction.setIcon(Application::getIconFont("FontAwesome").getIcon("undo"));
    _saveDefaultAction.setIcon(Application::getIconFont("FontAwesome").getIcon("save"));
    _loadFactoryDefaultAction.setIcon(Application::getIconFont("FontAwesome").getIcon("industry"));

    // Set tooltips
    _loadDefaultAction.setToolTip("Reset to default value");
    _saveDefaultAction.setToolTip("Save default value to disk");
    _loadFactoryDefaultAction.setToolTip("Load factory default value");

    // Load/save when triggered
    connect(&_loadDefaultAction, &TriggerAction::triggered, _widgetAction, &WidgetAction::loadDefault);
    connect(&_saveDefaultAction, &TriggerAction::triggered, _widgetAction, &WidgetAction::saveDefault);

    // Load factory default when triggered
    connect(&_loadFactoryDefaultAction, &TriggerAction::triggered, this, [this]() {
        _widgetAction->reset();
        _widgetAction->saveDefault();
    });

    const auto update = [this, widgetAction]() -> void {
        _label.setEnabled(widgetAction->isEnabled());
        _label.setText(QString("%1: ").arg(widgetAction->text()));
        _label.setToolTip(widgetAction->text());
        _label.setVisible(widgetAction->isVisible());
    };

    connect(widgetAction, &WidgetAction::changed, this, update);

    update();

    // Install event filter for intercepting label events
    _label.installEventFilter(this);
}

bool WidgetActionLabel::eventFilter(QObject* target, QEvent* event)
{
    if (_widgetAction->getSettingsPrefix().isEmpty())
        return QWidget::eventFilter(target, event);

    switch (event->type())
    {
        // Mouse button press event
        case QEvent::MouseButtonPress:
        {
            auto mouseButtonPress = static_cast<QMouseEvent*>(event);

            if (mouseButtonPress->button() != Qt::LeftButton)
                break;

            const auto isAtFactoryDefault   = _widgetAction->valueToVariant() == _widgetAction->defaultValueToVariant();
            const auto canSaveDefault       = isAtFactoryDefault ? false : _widgetAction->hasSavedDefault() && (_widgetAction->valueToVariant() != _widgetAction->savedDefaultValueToVariant());

            _loadDefaultAction.setEnabled(_widgetAction->isResettable());
            _saveDefaultAction.setEnabled(canSaveDefault);
            _loadFactoryDefaultAction.setEnabled(_widgetAction->isFactoryResettable());

            auto contextMenu = new QMenu();

            contextMenu->addAction(&_loadDefaultAction);
            contextMenu->addAction(&_saveDefaultAction);

            contextMenu->addSeparator();

            auto optionsMenu = new QMenu("Options");

            optionsMenu->setIcon(Application::getIconFont("FontAwesome").getIcon("cogs"));
            optionsMenu->addAction(&_loadFactoryDefaultAction);

            contextMenu->addMenu(optionsMenu);

            // Show the context menu
            contextMenu->exec(cursor().pos());

            break;
        }

        // Mouse enter event
        case QEvent::Enter:
        {
            setStyleSheet("QLabel { text-decoration: underline; }");

            break;
        }

        // Mouse leave event
        case QEvent::Leave:
        {
            setStyleSheet("QLabel { text-decoration: none; }");

            break;
        }

        default:
            break;
    }

    return QWidget::eventFilter(target, event);
}

}
}
