#pragma once

#include "WidgetAction.h"

class QCheckBox;

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
        Widget(QWidget* parent, CheckAction* checkAction);

    public:
        QHBoxLayout* getLayout() { return _layout; }
        QCheckBox* getCheckBox() { return _checkBox; }

    protected:
        QHBoxLayout*    _layout;
        QCheckBox*      _checkBox;

        friend class CheckAction;
    };

protected:
    QWidget* getWidget(QWidget* parent, const Widget::State& state = Widget::State::Standard) override;;

public:
    CheckAction(QObject* parent, const QString& title = "");
};

}
}