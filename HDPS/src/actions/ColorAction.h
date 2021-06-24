#pragma once

#include "WidgetAction.h"

class QWidget;
class ColorPickerPushButton;
class QPushButton;

namespace hdps {

namespace gui {

/**
 * Color widget action class
 *
 * Stores a color and creates widgets to interact with it
 *
 * @author Thomas Kroes
 */
class ColorAction : public WidgetAction
{
    Q_OBJECT

public:

    class Widget : public WidgetAction::Widget {
    protected:
        Widget(QWidget* parent, ColorAction* colorAction);

    public:
        QHBoxLayout* getLayout() { return _layout; }
        ColorPickerPushButton* getColorPickerPushButton() { return _colorPickerPushButton; }
        QPushButton* getResetPushButton() { return _resetPushButton; }

    protected:
        QHBoxLayout*            _layout;
        ColorPickerPushButton*  _colorPickerPushButton;
        QPushButton*            _resetPushButton;

        friend class ColorAction;
    };

protected:
    QWidget* getWidget(QWidget* parent, const Widget::State& state = Widget::State::Standard) override {
        return new ColorAction::Widget(parent, this);
    };

public:
    ColorAction(QObject* parent, const QString& title = "", const QColor& color = DEFAULT_COLOR, const QColor& defaultColor = DEFAULT_COLOR);

    QColor getColor() const;
    void setColor(const QColor& color);

    QColor getDefaultColor() const;
    void setDefaultColor(const QColor& defaultColor);

    bool canReset() const;
    void reset();

    ColorAction& operator= (const ColorAction& other)
    {
        WidgetAction::operator=(other);

        _color          = other._color;
        _defaultColor   = other._defaultColor;

        return *this;
    }

signals:
    void colorChanged(const QColor& color);
    void defaultColorChanged(const QColor& defaultColor);

protected:
    QColor  _color;
    QColor  _defaultColor;

    static const QColor DEFAULT_COLOR;
};

}
}