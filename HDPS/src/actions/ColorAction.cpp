#include "ColorAction.h"
#include "Application.h"

#include <QDebug>
#include <QPainter>

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

    _toolButton.setToolTip(colorAction->toolTip());
    _toolButton.addAction(&_colorPickerAction);
    _toolButton.setPopupMode(QToolButton::InstantPopup);
    _toolButton.setFixedHeight(24);
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

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    const auto margin = 5;

    QPoint offset(margin, margin);

    auto fillRect = rect();

    fillRect.setTopLeft(fillRect.topLeft() + offset);
    fillRect.setBottomRight(fillRect.bottomRight() - offset - QPoint(0, 0));

    auto color = _colorPickerAction.getColor();

    if (!isEnabled()) {
        const auto grayScale = qGray(color.rgb());
        color.setRgb(grayScale, grayScale, grayScale);
    }

    painter.setBrush(QBrush(color));
    painter.setPen(QPen(QBrush(isEnabled() ? QColor(50, 50, 50) : Qt::gray), 1.0, Qt::SolidLine, Qt::SquareCap, Qt::SvgMiterJoin));
    painter.drawRect(fillRect);
}

}
}
