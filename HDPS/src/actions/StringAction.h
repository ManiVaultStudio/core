#pragma once

#include "WidgetAction.h"

#include <QHBoxLayout>
#include <QLineEdit>

class QWidget;

namespace hdps {

namespace gui {

/**
 * String widget action class
 *
 * Stores a string creates widgets to interact with it
 *
 * @author Thomas Kroes
 */
class StringAction : public WidgetAction
{
    Q_OBJECT

public:
    class Widget : public WidgetAction::Widget {
    public:
        Widget(QWidget* widget, StringAction* stringAction);

    private:
        QHBoxLayout     _layout;
        QLineEdit       _lineEdit;
    };

public:
    StringAction(QObject* parent, const QString& title = "");

    QWidget* createWidget(QWidget* parent) override;

    QString getValue() const;
    void setValue(const QString& value);

signals:
    void valueChanged(const QString& value);

protected:
    QString     _value;
};

}
}