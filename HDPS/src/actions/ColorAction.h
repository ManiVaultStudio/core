#pragma once

#include "WidgetAction.h"

#include "../widgets/ColorPickerPushButton.h"

#include <QHBoxLayout>
#include <QComboBox>

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
        Widget(QWidget* widget, ColorAction* colorAction);

        ColorPickerPushButton& getColorPickerPushButton() {
            return _colorPickerPushButton;
        }

    private:
        QHBoxLayout             _layout;
        ColorPickerPushButton   _colorPickerPushButton;
    };

public:
    ColorAction(QObject* parent, const QString& title = "", const QColor& color = Qt::gray);

    QWidget* createWidget(QWidget* parent) override;

    QColor getColor() const;
    void setColor(const QColor& color);

signals:
    void colorChanged(const QColor& color);

protected:
    QColor      _color;
};

}
}