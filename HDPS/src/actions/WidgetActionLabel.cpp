#include "WidgetActionLabel.h"
#include "WidgetAction.h"
#include "Application.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QMenu>
#include <QInputDialog >

namespace hdps {

namespace gui {

WidgetActionLabel::WidgetActionLabel(WidgetAction* widgetAction, QWidget* parent /*= nullptr*/, Qt::WindowFlags windowFlags /*= Qt::WindowFlags()*/) :
    QWidget(parent, windowFlags),
    _widgetAction(widgetAction),
    _label(),
    _publishAction(this, "Publish"),
    _unPublishAction(this, "Un-publish")
{
    _publishAction.setIcon(Application::getIconFont("FontAwesome").getIcon("link"));
    _unPublishAction.setIcon(Application::getIconFont("FontAwesome").getIcon("unlink"));

    connect(&_publishAction, &TriggerAction::triggered, this, [this]() {
        const auto parameterName = QInputDialog::getText(&_label, "Publish parameter", "Parameter name                                                                ", QLineEdit::Normal, _widgetAction->getSettingsPath());

        if (!parameterName.isEmpty())
            _widgetAction->publish(parameterName);
    });

    connect(&_publishAction, &TriggerAction::triggered, _widgetAction, &WidgetAction::unPublish);

    auto layout = new QVBoxLayout();

    layout->setAlignment(Qt::AlignRight | Qt::AlignCenter);

    layout->setMargin(0);
    layout->addStretch(1);
    layout->addWidget(&_label);
    layout->addStretch(1);

    setLayout(layout);

    _label.setAlignment(Qt::AlignRight);

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
    auto contextMenu = new QMenu();

    if (_widgetAction->mayPublish()) {
        if (_widgetAction->isPublic())
            contextMenu->addAction(&_unPublishAction);
        else
            contextMenu->addAction(&_publishAction);
    }

    if (contextMenu->actions().isEmpty())
        return QWidget::eventFilter(target, event);

    switch (event->type())
    {
        // Mouse button press event
        case QEvent::MouseButtonPress:
        {
            auto mouseButtonPress = static_cast<QMouseEvent*>(event);

            if (mouseButtonPress->button() != Qt::LeftButton)
                break;

            contextMenu->exec(cursor().pos());

            break;
        }

        case QEvent::Enter:
        {
            setStyleSheet("QLabel { text-decoration: underline; }");

            break;
        }

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
