#pragma once

#include "WidgetAction.h"

#include "../widgets/ColorPickerPushButton.h"

class QWidget;

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
    public:
        Widget(QWidget* widget, ColorAction* colorAction, const bool& resettable = false);
    };

public:
    ColorAction(QObject* parent, const QString& title = "", const QColor& color = DEFAULT_COLOR, const QColor& defaultColor = DEFAULT_COLOR);

    QWidget* createWidget(QWidget* parent) override;

    QColor getColor() const;
    void setColor(const QColor& color);

    QColor getDefaultColor() const;
    void setDefaultColor(const QColor& defaultColor);

    bool canReset() const;
    void reset();

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