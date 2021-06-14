#pragma once

#include "WidgetAction.h"

class QPushButton;

namespace hdps {

namespace gui {

/**
 * Toggle action class
 *
 * Toggle action with toggle button UI
 *
 * @author Thomas Kroes
 */
class ToggleAction : public WidgetAction
{
    Q_OBJECT

public:

    class Widget : public WidgetAction::Widget {
    protected:
        Widget(QWidget* parent, ToggleAction* toggleAction);

    public:
        QHBoxLayout* getLayout() { return _layout; }
        QPushButton* getPushButton() { return _pushButton; }

    protected:
        QHBoxLayout*    _layout;
        QPushButton*    _pushButton;

        friend class ToggleAction;
    };

protected:
    QWidget* getWidget(QWidget* parent, const Widget::State& state = Widget::State::Standard) override;;

public:
    ToggleAction(QObject* parent, const QString& title = "");
};

}
}