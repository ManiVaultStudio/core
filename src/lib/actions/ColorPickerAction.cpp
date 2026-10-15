// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ColorPickerAction.h"
#include "WidgetActionLabel.h"
#include "Application.h"

#include <QDebug>
#include <QHBoxLayout>

namespace mv {

namespace gui {

const QColor ColorPickerAction::DEFAULT_COLOR = Qt::gray;

ColorPickerAction::ColorPickerAction(QObject* parent, const QString& title, const QColor& color /*= DEFAULT_COLOR*/) :
    WidgetAction(parent, title),
    _color()
{
    setText(title);
    setColor(color);
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

void ColorPickerAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicColorPickerAction = dynamic_cast<ColorPickerAction*>(publicAction);

    Q_ASSERT(publicColorPickerAction != nullptr);

    if (publicColorPickerAction == nullptr)
        return;

    connect(this, &ColorPickerAction::colorChanged, publicColorPickerAction, &ColorPickerAction::setColor);
    connect(publicColorPickerAction, &ColorPickerAction::colorChanged, this, &ColorPickerAction::setColor);

    setColor(publicColorPickerAction->getColor());

    WidgetAction::connectToPublicAction(publicAction, recursive);
}

void ColorPickerAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    auto publicColorPickerAction = dynamic_cast<ColorPickerAction*>(getPublicAction());

    Q_ASSERT(publicColorPickerAction != nullptr);

    if (publicColorPickerAction == nullptr)
        return;

    disconnect(this, &ColorPickerAction::colorChanged, publicColorPickerAction, &ColorPickerAction::setColor);
    disconnect(publicColorPickerAction, &ColorPickerAction::colorChanged, this, &ColorPickerAction::setColor);

    WidgetAction::disconnectFromPublicAction(recursive);
}

ColorPickerAction::Widget::Widget(QWidget* parent, ColorPickerAction* colorPickerAction) :
    WidgetActionWidget(parent, colorPickerAction),
    _layout(),
    _colorDialog(),
    _hueAction(this, "Hue", 0, 359, colorPickerAction->getColor().hue()),
    _saturationAction(this, "Saturation", 0, 255, colorPickerAction->getColor().hslSaturation()),
    _lightnessAction(this, "Lightness", 0, 255, colorPickerAction->getColor().lightness()),
    _updateColorPickerAction(true)
{
    setAcceptDrops(true);

    _colorDialog.setCurrentColor(colorPickerAction->getColor());
    _colorDialog.setOptions(QColorDialog::DontUseNativeDialog | QColorDialog::ShowAlphaChannel);
    
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

    if(colorPickerWidget == nullptr)
        colorPickerWidget = getWidgetFromColorDialog("QtPrivate::QColorPicker");
    if(colorLuminanceWidget == nullptr)
        colorLuminanceWidget = getWidgetFromColorDialog("QtPrivate::QColorLuminancePicker");

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
            _saturationAction.setValue(color.hslSaturation());
            _lightnessAction.setValue(color.lightness());
        }
        _updateColorPickerAction = true;
    });

    connect(&_colorDialog, &QColorDialog::currentColorChanged, this, [this, colorPickerAction](const QColor& color) {
        if (!_updateColorPickerAction)
            return;

        colorPickerAction->setColor(color);
    });

    setLayout(&_layout);
}

}
}
