#pragma once

#include "WidgetAction.h"

class QCheckBox;
class QPushButton;

namespace hdps {

namespace gui {

/**
 * Toggle action class
 *
 * Toggle action with check/toggle button UI
 *
 * @author Thomas Kroes
 */
class ToggleAction : public WidgetAction
{
    Q_OBJECT

public:

    enum class Mode {
        CheckBox,
        Button

    };
    class Widget : public WidgetAction::Widget {
    protected:
        Widget(QWidget* parent, ToggleAction* checkAction, const Mode& mode = Mode::CheckBox);

    public:
        QHBoxLayout* getLayout() { return _layout; }
        QCheckBox* getCheckBox() { return _checkBox; }
        QPushButton* getPushButton() { return _pushButton; }

    protected:
        QHBoxLayout*    _layout;
        QCheckBox*      _checkBox;
        QPushButton*    _pushButton;

        friend class ToggleAction;
    };

protected:
    QWidget* getWidget(QWidget* parent, const Widget::State& state = Widget::State::Standard) override;;

public:
    ToggleAction(QObject* parent, const QString& title = "");

    Widget* createCheckBoxWidget(QWidget* parent) { return new Widget(parent, this);  };
    Widget* createPushButtonWidget(QWidget* parent) { return new Widget(parent, this, Mode::Button);  };
};

}
}