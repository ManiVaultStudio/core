#pragma once

#include "WidgetAction.h"

class QCheckBox;
class QPushButton;

namespace hdps {

namespace gui {

/**
 * Check action class
 *
 * Check action with check button UI
 *
 * @author Thomas Kroes
 */
class CheckAction : public WidgetAction
{
    Q_OBJECT

public:

    class Widget : public WidgetAction::Widget {
    protected:
        Widget(QWidget* parent, CheckAction* checkAction, const bool& toggleButton = false);

    public:
        QHBoxLayout* getLayout() { return _layout; }
        QCheckBox* getCheckBox() { return _checkBox; }

    protected:
        QHBoxLayout*    _layout;
        QCheckBox*      _checkBox;
        QPushButton*    _pushButton;

        friend class CheckAction;
    };

protected:
    QWidget* getWidget(QWidget* parent, const Widget::State& state = Widget::State::Standard) override;;

public:
    CheckAction(QObject* parent, const QString& title = "");

    Widget* createCheckBoxWidget(QWidget* parent) { return new Widget(parent, this);  };
    Widget* createToggleButtonWidget(QWidget* parent) { return new Widget(parent, this, true);  };
};

}
}