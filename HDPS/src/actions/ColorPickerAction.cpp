#include "ColorPickerAction.h"
#include "Application.h"

#include <QDebug>
#include <QHBoxLayout>

namespace hdps {

namespace gui {

const QColor ColorPickerAction::DEFAULT_COLOR = Qt::gray;

ColorPickerAction::ColorPickerAction(QObject* parent, const QString& title /*= ""*/, const QColor& color /*= DEFAULT_COLOR*/, const QColor& defaultColor /*= DEFAULT_COLOR*/) :
    WidgetAction(parent),
    _color()
{
    setText(title);
    initialize(color, defaultColor);
}

void ColorPickerAction::initialize(const QColor& color /*= DEFAULT_COLOR*/, const QColor& defaultColor /*= DEFAULT_COLOR*/)
{
    setColor(color);
    setDefaultColor(defaultColor);
}

QColor ColorPickerAction::getColor() const
{
    return _color;
}

void ColorPickerAction::setColor(const QColor& color)
{
    if (color == _color)
        return;

    _color = color;

    emit colorChanged(_color);
}

QColor ColorPickerAction::getDefaultColor() const
{
    return _defaultColor;
}

void ColorPickerAction::setDefaultColor(const QColor& defaultColor)
{
    if (defaultColor == _defaultColor)
        return;

    _defaultColor = defaultColor;

    emit defaultColorChanged(_defaultColor);
}

bool ColorPickerAction::canReset() const
{
    return _color != _defaultColor;
}

void ColorPickerAction::reset()
{
    setColor(_defaultColor);
}

ColorPickerAction::Widget::Widget(QWidget* parent, ColorPickerAction* colorPickerAction, const WidgetActionWidget::State& state) :
    WidgetActionWidget(parent, colorPickerAction, state),
    _layout(),
    _colorDialog(),
    _hueAction(this, "Hue", 0, 359, colorPickerAction->getColor().hue(), colorPickerAction->getColor().hue()),
    _saturationAction(this, "Saturation", 0, 255, colorPickerAction->getColor().saturation(), colorPickerAction->getColor().saturation()),
    _lightnessAction(this, "Lightness", 0, 255, colorPickerAction->getColor().lightness(), colorPickerAction->getColor().lightness()),
    _updateColorPickerAction(true)
{
    setAcceptDrops(true);

    _colorDialog.setCurrentColor(colorPickerAction->getColor());

    const auto getWidgetFromColorDialog = [this](const QString& name) -> QWidget* {
        auto allChildWidgets = _colorDialog.findChildren<QWidget *>();

        for (auto widget : allChildWidgets) {
            if (strcmp(widget->metaObject()->className(), name.toLatin1()) == 0)
                return widget;
        }

        return nullptr;
    };

    auto colorPickerWidget      = getWidgetFromColorDialog("QColorPicker");
    auto colorLuminanceWidget   = getWidgetFromColorDialog("QColorLuminancePicker");

    colorPickerWidget->setFixedSize(250, 150);

    auto pickersLayout = new QHBoxLayout();

    pickersLayout->addWidget(colorPickerWidget);
    pickersLayout->addWidget(colorLuminanceWidget);

    _layout.addLayout(pickersLayout);

    auto hslLayout = new QGridLayout();

    hslLayout->addWidget(_hueAction.createLabelWidget(this), 0, 0);
    hslLayout->addWidget(_hueAction.createWidget(this), 0, 1);
    hslLayout->addWidget(_saturationAction.createLabelWidget(this), 1, 0);
    hslLayout->addWidget(_saturationAction.createWidget(this), 1, 1);
    hslLayout->addWidget(_lightnessAction.createLabelWidget(this), 2, 0);
    hslLayout->addWidget(_lightnessAction.createWidget(this), 2, 1);

    _layout.addLayout(hslLayout);

    const auto updateColorFromHSL = [this, colorPickerAction]() -> void {
        if (!_updateColorPickerAction)
            return;

        colorPickerAction->setColor(QColor::fromHsl(_hueAction.getValue(), _saturationAction.getValue(), _lightnessAction.getValue()));
    };

    connect(&_hueAction, &IntegralAction::valueChanged, this, [this, updateColorFromHSL](const std::int32_t& value) {
        updateColorFromHSL();
    });

    connect(&_saturationAction, &IntegralAction::valueChanged, this, [this, updateColorFromHSL](const std::int32_t& value) {
        updateColorFromHSL();
    });

    connect(&_lightnessAction, &IntegralAction::valueChanged, this, [this, updateColorFromHSL](const std::int32_t& value) {
        updateColorFromHSL();
    });

    connect(colorPickerAction, &ColorPickerAction::colorChanged, this, [this, colorPickerAction](const QColor& color) {
        _updateColorPickerAction = false;
        {
            _colorDialog.setCurrentColor(color);
            _hueAction.setValue(color.hue());
            _saturationAction.setValue(color.saturation());
            _lightnessAction.setValue(color.lightness());
        }
        _updateColorPickerAction = true;
    });

    connect(&_colorDialog, &QColorDialog::currentColorChanged, this, [this, colorPickerAction](const QColor& color) {
        if (!_updateColorPickerAction)
            return;

        colorPickerAction->setColor(color);
    });

    switch (state)
    {
        case Widget::State::Standard:
        {
            setLayout(&_layout);
            break;
        }

        case Widget::State::Popup:
        {
            setPopupLayout(&_layout);
            break;
        }

        default:
            break;
    }
}

}
}
