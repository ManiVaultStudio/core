#include "ColorAction.h"
#include "Application.h"

#include <QDebug>
#include <QPainter>
#include <QStyleOption>

namespace hdps {

namespace gui {

const QColor ColorAction::DEFAULT_COLOR = Qt::gray;

ColorAction::ColorAction(QObject* parent, const QString& title /*= ""*/, const QColor& color /*= DEFAULT_COLOR*/, const QColor& defaultColor /*= DEFAULT_COLOR*/) :
    WidgetAction(parent),
    _color(),
    _defaultColor()
{
    setText(title);
    initialize(color, defaultColor);
}

void ColorAction::initialize(const QColor& color /*= DEFAULT_COLOR*/, const QColor& defaultColor /*= DEFAULT_COLOR*/)
{
    setColor(color);
    setDefaultColor(defaultColor);
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

ColorAction::ColorPickerPushButtonWidget::ColorPickerPushButtonWidget(QWidget* parent, ColorAction* colorAction) :
    WidgetActionWidget(parent, colorAction, WidgetActionWidget::State::Standard),
    _layout(),
    _colorPickerAction(this, "Color picker", colorAction->getColor(), colorAction->getColor()),
    _toolButton(this, _colorPickerAction)
{
    setAcceptDrops(true);

    connect(&_colorPickerAction, &ColorPickerAction::colorChanged, this, [this, colorAction](const QColor& color) {
        colorAction->setColor(color);
        _toolButton.update();
    });

    _toolButton.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    _toolButton.setToolTip(colorAction->toolTip());
    _toolButton.addAction(&_colorPickerAction);
    _toolButton.setPopupMode(QToolButton::InstantPopup);
    _toolButton.setStyleSheet("QToolButton::menu-indicator { image: none; }");

    _layout.setMargin(0);
    _layout.addWidget(&_toolButton);

    setLayout(&_layout);
}

ColorAction::ColorPickerPushButtonWidget::ToolButton::ToolButton(QWidget* parent, ColorPickerAction& colorPickerAction) :
    QToolButton(parent),
    _colorPickerAction(colorPickerAction)
{
}

void ColorAction::ColorPickerPushButtonWidget::ToolButton::paintEvent(QPaintEvent* paintEvent)
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

    styleOption.init(this);

    // Set inset margins
    const auto margin = 10;

    // Rect offset
    QPoint offset(margin, margin);

    // Deflated fill rectangle for color inset
    auto colorRect = pixmapRect.marginsRemoved(QMargins(margin, margin, margin + 1, margin));

    // Get current color
    auto color = _colorPickerAction.getColor();

    // Support enabled/disabled control
    if (!isEnabled()) {
        const auto grayScale = qGray(color.rgb());
        color.setRgb(grayScale, grayScale, grayScale);
    }

    // Establish pen color based on whether the color map is enabled or not
    const auto penColor = isEnabled() ? styleOption.palette.color(QPalette::Normal, QPalette::ButtonText) : styleOption.palette.color(QPalette::Disabled, QPalette::ButtonText);

    // Do the painting
    painterColorPixmap.setBrush(QBrush(color));
    painterColorPixmap.setPen(QPen(penColor, 2.0, Qt::SolidLine, Qt::SquareCap, Qt::SvgMiterJoin));
    painterColorPixmap.drawRoundedRect(colorRect, 6, 6);

    QPainter painterColorWidget(this);

    painterColorWidget.drawPixmap(rect(), colorPixmap, pixmapRect);
}

}
}
