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
    setWidgetFlags(WidgetFlag::Basic);
}

void ColorAction::initialize(const QColor& color /*= DEFAULT_COLOR*/, const QColor& defaultColor /*= DEFAULT_COLOR*/)
{
    setColor(color);
    setDefaultColor(defaultColor);

    setResettable(isResettable());
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

    setResettable(isResettable());
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

bool ColorAction::isResettable() const
{
    return _color != _defaultColor;
}

void ColorAction::reset()
{
    setColor(_defaultColor);
}

ColorAction::PushButtonWidget::PushButtonWidget(QWidget* parent, ColorAction* colorAction) :
    WidgetActionWidget(parent, colorAction, WidgetActionWidget::State::Standard),
    _layout(),
    _colorPickerAction(this, "Color picker", colorAction->getColor(), colorAction->getColor()),
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

    _layout.setMargin(0);
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

    styleOption.init(this);

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
    painterColorPixmap.setPen(QPen(penColor, 2.0, Qt::SolidLine, Qt::SquareCap, Qt::SvgMiterJoin));
    painterColorPixmap.drawRoundedRect(colorRect, 5, 5);

    QPainter painterColorWidget(this);

    painterColorWidget.drawPixmap(rect(), colorPixmap, pixmapRect);
}

QWidget* ColorAction::getWidget(QWidget* parent, const WidgetActionWidget::State& state /*= WidgetActionWidget::State::Standard*/)
{
    auto widget = new QWidget(parent);
    auto layout = new QHBoxLayout();

    layout->setMargin(0);
    layout->setSpacing(3);

    if (hasWidgetFlag(WidgetFlag::Picker))
        layout->addWidget(new PushButtonWidget(parent, this));

    if (hasWidgetFlag(WidgetFlag::ResetPushButton))
        layout->addWidget(createResetButton(parent));

    widget->setLayout(layout);

    const auto update = [this, widget]() -> void {
        widget->setEnabled(isEnabled());
        widget->setToolTip(text());
    };

    connect(this, &IntegralAction::changed, this, [update]() {
        update();
    });

    update();

    return widget;
}

}
}
