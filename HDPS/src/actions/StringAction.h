#pragma once

#include "WidgetAction.h"

class QWidget;
class QLineEdit;
class QPushButton;

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
    protected:
        Widget(QWidget* parent, StringAction* stringAction);

    public:
        QHBoxLayout* getLayout() { return _layout; }
        QLineEdit* getLineEdit() { return _lineEdit; }
        QPushButton* getResetPushButton() { return _resetPushButton; }

    protected:
        QHBoxLayout*    _layout;
        QLineEdit*      _lineEdit;
        QPushButton*    _resetPushButton;

        friend class StringAction;
    };

protected:
    QWidget* getWidget(QWidget* parent, const Widget::State& state = Widget::State::Standard) override {
        return new StringAction::Widget(parent, this);
    };

public:
    StringAction(QObject* parent, const QString& title = "");

    QString getString() const;
    void setString(const QString& string);

    QString getDefaultString() const;
    void setDefaultString(const QString& defaultString);

    bool canReset() const;
    void reset();

    StringAction& operator= (const StringAction& other)
    {
        _string         = other._string;
        _defaultString  = other._defaultString;

        return *this;
    }

signals:
    void stringChanged(const QString& string);
    void defaultStringChanged(const QString& defaultString);

protected:
    QString     _string;
    QString     _defaultString;
};

}
}