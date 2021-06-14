#pragma once

#include "WidgetAction.h"

class QPushButton;

namespace hdps {

namespace gui {

/**
 * Trigger action class
 *
 * Check action with button UI
 *
 * @author Thomas Kroes
 */
class TriggerAction : public WidgetAction
{
    Q_OBJECT

public:

    class Widget : public WidgetAction::Widget {
    protected:
        Widget(QWidget* parent, TriggerAction* triggerAction);

    public:
        QHBoxLayout* getLayout() { return _layout; }
        QPushButton* getPushButton() { return _pushButton; }

    protected:
        QHBoxLayout*    _layout;
        QPushButton*    _pushButton;

        friend class TriggerAction;
    };

protected:
    QWidget* getWidget(QWidget* parent, const Widget::State& state = Widget::State::Standard) override;;

public:
    TriggerAction(QObject* parent, const QString& title = "");
};

}
}