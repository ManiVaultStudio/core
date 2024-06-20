// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ColorAction.h"
#include "WidgetActionExampleWidget.h"
#include "Application.h"

#include <QDebug>
#include <QPainter>
#include <QStyleOption>

using namespace mv::util;

namespace mv::gui {

const QColor ColorAction::DEFAULT_COLOR = Qt::gray;

ColorAction::ColorAction(QObject* parent, const QString& title, const QColor& color /*= DEFAULT_COLOR*/) :
    WidgetAction(parent, title),
    _color()
{
    setText(title);
    setDefaultWidgetFlags(WidgetFlag::Basic);
    setColor(color);
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

    saveToSettings();
}

void ColorAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicColorAction = dynamic_cast<ColorAction*>(publicAction);

    Q_ASSERT(publicColorAction != nullptr);

    if (publicColorAction == nullptr)
        return;

    connect(this, &ColorAction::colorChanged, publicColorAction, &ColorAction::setColor);
    connect(publicColorAction, &ColorAction::colorChanged, this, &ColorAction::setColor);

    setColor(publicColorAction->getColor());

    WidgetAction::connectToPublicAction(publicAction, recursive);
}

void ColorAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    auto publicColorAction = dynamic_cast<ColorAction*>(getPublicAction());

    Q_ASSERT(publicColorAction != nullptr);

    if (publicColorAction == nullptr)
        return;

    disconnect(this, &ColorAction::colorChanged, publicColorAction, &ColorAction::setColor);
    disconnect(publicColorAction, &ColorAction::colorChanged, this, &ColorAction::setColor);

    WidgetAction::disconnectFromPublicAction(recursive);
}

void ColorAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "Value");

    setColor(variantMap["Value"].value<QColor>());
}

QVariantMap ColorAction::toVariantMap() const
{
    QVariantMap variantMap = WidgetAction::toVariantMap();

    variantMap.insert({
        { "Value", QVariant::fromValue(_color) }
    });

    return variantMap;
}

ColorAction::PushButtonWidget::PushButtonWidget(QWidget* parent, ColorAction* colorAction) :
    WidgetActionWidget(parent, colorAction),
    _layout(),
    _colorPickerAction(this, "Color picker", colorAction->getColor()),
    _toolButton(this, _colorPickerAction)
{
    setAcceptDrops(true);
    setObjectName("PushButton");

    connect(&_colorPickerAction, &ColorPickerAction::colorChanged, this, [this, colorAction](const QColor& color) {
        colorAction->setColor(color);
        _toolButton.update();
    });

    connect(colorAction, &ColorAction::colorChanged, this, [this](const QColor& color) {
        _colorPickerAction.setColor(color);
    });

    _toolButton.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    _toolButton.setToolTip(colorAction->toolTip());
    _toolButton.addAction(&_colorPickerAction);
    _toolButton.setPopupMode(QToolButton::InstantPopup);
    _toolButton.setStyleSheet("QToolButton::menu-indicator { image: none; }");

    _layout.setContentsMargins(0, 0, 0, 0);
    _layout.addWidget(&_toolButton);

    setLayout(&_layout);
}

ColorAction::PushButtonWidget::ToolButton::ToolButton(QWidget* parent, ColorPickerAction& colorPickerAction) :
    QToolButton(parent),
    _colorPickerAction(colorPickerAction)
{
    setObjectName("ToolButton");
}

void ColorAction::PushButtonWidget::ToolButton::paintEvent(QPaintEvent* paintEvent)
{
    QToolButton::paintEvent(paintEvent);

    // Draw at a higher resolution to get better anti-aliasing
    const auto pixmapSize = 2 * size();
    const auto pixmapRect = QRect(QPoint(), pixmapSize);

    // Create color pixmap
    QPixmap colorPixmap(pixmapSize);

    // Fill with a transparent background
    colorPixmap.fill(Qt::transparent);

    // Create a painter to draw in the color pixmap
    QPainter painterColorPixmap(&colorPixmap);
    
    // Enable anti-aliasing
    painterColorPixmap.setRenderHint(QPainter::Antialiasing);

    QStyleOption styleOption;

    styleOption.initFrom(this);

    // Set inset margins
    const auto margin = 10;

    // Rect offset
    QPoint offset(margin, margin);

    // Deflated fill rectangle for color inset
    auto colorRect = pixmapRect.marginsRemoved(QMargins(margin, margin, margin + 1, margin + 1));

    // Get current color
    auto color = _colorPickerAction.getColor();

    // Support enabled/disabled control
    if (!isEnabled()) {
        const auto grayScale = qGray(color.rgb());
        color.setRgb(grayScale, grayScale, grayScale);
    }

    // Establish pen color based on whether the color map is enabled or not
    const auto penColor = isEnabled() ? styleOption.palette.color(QPalette::Normal, QPalette::Shadow) : styleOption.palette.color(QPalette::Disabled, QPalette::ButtonText);

    // Do the painting
    painterColorPixmap.setBrush(QBrush(color));
    painterColorPixmap.setPen(QPen(penColor, 1.5, Qt::SolidLine, Qt::SquareCap, Qt::SvgMiterJoin));
    painterColorPixmap.drawRoundedRect(colorRect, 4, 4);

    QPainter painterColorWidget(this);

    painterColorWidget.drawPixmap(rect(), colorPixmap, pixmapRect);
}

QWidget* ColorAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    if (widgetFlags & WidgetFlag::Picker)
        layout->addWidget(new PushButtonWidget(parent, this));

    widget->setLayout(layout);

    return widget;
}

QWidget* ColorAction::createExampleWidget(QWidget* parent) const
{
    auto exampleWidget  = new WidgetActionExampleWidget(parent);
    auto colorAction    = new ColorAction(exampleWidget, "Example");

    const QString markdownText = 
        "## General \n\n"
        "[#include <actions/ColorAction.h>](https://github.com/ManiVaultStudio/core/blob/master/ManiVault/src/actions/ColorAction.h) \n\n"
        "`mv::gui::ColorAction` provides a color picking GUI with which the `hue`, `saturation`, `lightness` and `darkness` can be modified. \n\n"
        "## Usage \n\n"
        "Click on the color button and modify the color by dragging: \n\n"
        "- The color cross hair in the large square \n\n"
        "- The darkness slider up/down at the far right \n\n"
        "- The hue slider left/right \n\n"
        "- The saturation slider left/right \n\n"
        "- The lightness slider left/right \n\n"
        "## API \n\n"
        "The current color can be retrieved with `getColor()` and set with `setColor(...)`.";

    exampleWidget->addWidget(colorAction->createWidget(exampleWidget));
    exampleWidget->addMarkDownSection(markdownText);

    return exampleWidget;
}

}
