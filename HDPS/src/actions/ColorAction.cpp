#include "ColorAction.h"
#include "Application.h"

#include <QDebug>
#include <QHBoxLayout>

namespace hdps {

namespace gui {

const QColor ColorAction::DEFAULT_COLOR = Qt::gray;

ColorAction::ColorAction(QObject* parent, const QString& title /*= ""*/, const QColor& color /*= DEFAULT_COLOR*/, const QColor& defaultColor /*= DEFAULT_COLOR*/) :
    WidgetAction(parent),
    _color(),
    _defaultColor()
{
    setText(title);
    setColor(color);
    setDefaultColor(defaultColor);
}

QWidget* ColorAction::createWidget(QWidget* parent)
{
    return new Widget(parent, this);
}

QColor ColorAction::getColor() const
{
    return _color;
}

void ColorAction::setColor(const QColor& color)
{
    if (color == _color)
        return;

    _color = color;

    emit colorChanged(_color);
}

QColor ColorAction::getDefaultColor() const
{
    return _defaultColor;
}

void ColorAction::setDefaultColor(const QColor& defaultColor)
{
    if (defaultColor == _defaultColor)
        return;

    _defaultColor = defaultColor;

    emit defaultColorChanged(_defaultColor);
}

bool ColorAction::canReset() const
{
    return _color != _defaultColor;
}

void ColorAction::reset()
{
    setColor(_defaultColor);
}

ColorAction::Widget::Widget(QWidget* parent, ColorAction* colorAction, const bool& resettable /*= true*/) :
    WidgetAction::Widget(parent, colorAction)
{
    auto layout = new QHBoxLayout();
    auto colorPickerPushButton = new ColorPickerPushButton();

    layout->setMargin(0);
    layout->addWidget(colorPickerPushButton);

    setLayout(layout);

    connect(colorPickerPushButton, &ColorPickerPushButton::colorChanged, this, [this, colorAction](const QColor& color) {
        colorAction->setColor(color);
    });

    const auto updateColorPickerPushButton = [this, colorAction, colorPickerPushButton]() -> void {
        colorPickerPushButton->setColor(colorAction->getColor());
    };

    connect(colorAction, &ColorAction::colorChanged, this, [this, updateColorPickerPushButton](const QColor& color) {
        updateColorPickerPushButton();
    });

    updateColorPickerPushButton();

    if (resettable) {
        auto resetPushButton = new QPushButton();

        resetPushButton->setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("undo"));
        resetPushButton->setToolTip(QString("Reset %1").arg(colorAction->text()));

        layout->addWidget(resetPushButton);

        connect(resetPushButton, &QPushButton::clicked, this, [this, colorAction]() {
            colorAction->reset();
        });

        const auto onUpdateColor = [resetPushButton, colorAction]() -> void {
            resetPushButton->setEnabled(colorAction->canReset());
        };

        connect(colorAction, &ColorAction::colorChanged, this, [this, onUpdateColor](const QColor& color) {
            onUpdateColor();
        });

        onUpdateColor();
    }
}

}
}