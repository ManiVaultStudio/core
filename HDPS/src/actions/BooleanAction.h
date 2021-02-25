#pragma once

#include "WidgetAction.h"

#include <QHBoxLayout>
#include <QCheckBox>

class QWidget;

namespace hdps {

namespace gui {

/**
 * Boolean widget action class
 *
 * Stores a boolean and creates widgets to interact with it
 *
 * @author Thomas Kroes
 */
class BooleanAction : public WidgetAction
{
    Q_OBJECT

public:
    class Widget : public WidgetAction::Widget {
    public:
        Widget(QWidget* widget, BooleanAction* optionAction);

    private:
        QHBoxLayout     _layout;
        QCheckBox       _checkbox;
    };

public:
    BooleanAction(QObject* parent, const QString& title = "");

    QWidget* createWidget(QWidget* parent) override;

    bool getValue() const {
        return _value;
    }

    void setValue(const bool& value) {
        if (value == _value)
            return;

        _value = value;

        emit valueChanged(_value);
    }

signals:
    void valueChanged(const bool& value);

protected:
    bool    _value;
};

}
}