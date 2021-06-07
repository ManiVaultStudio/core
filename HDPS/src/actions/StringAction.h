#pragma once

#include "WidgetAction.h"

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
        Widget(QWidget* widget, StringAction* stringAction, const bool& resettable = true);
    };

public:
    StringAction(QObject* parent, const QString& title = "");

    QWidget* createWidget(QWidget* parent) override;

    QString getString() const;
    void setString(const QString& string);

    QString getDefaultString() const;
    void setDefaultString(const QString& defaultString);

    bool canReset() const;
    void reset();

signals:
    void stringChanged(const QString& string);
    void defaultStringChanged(const QString& defaultString);

protected:
    QString     _string;
    QString     _defaultString;
};

}
}