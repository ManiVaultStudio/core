#include "ColorAction.h"
#include "Application.h"

#include <QDebug>
#include <QPainter>
#include <QStyleOption>

using namespace hdps::util;

namespace hdps::gui {

const QColor ColorAction::DEFAULT_COLOR = Qt::gray;

ColorAction::ColorAction(QObject* parent, const QString& title /*= ""*/, const QColor& color /*= DEFAULT_COLOR*/, const QColor& defaultColor /*= DEFAULT_COLOR*/) :
    WidgetAction(parent),
    _color(),
    _defaultColor()
{
    setText(title);
    initialize(color, defaultColor);
    setDefaultWidgetFlags(WidgetFlag::Basic);
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

void ColorAction::connectToPublicAction(WidgetAction* publicAction)
{
    auto publicColorAction = dynamic_cast<ColorAction*>(publicAction);

    Q_ASSERT(publicColorAction != nullptr);

    connect(this, &ColorAction::colorChanged, publicColorAction, &ColorAction::setColor);
    connect(publicColorAction, &ColorAction::colorChanged, this, &ColorAction::setColor);

    setColor(publicColorAction->getColor());

    WidgetAction::connectToPublicAction(publicAction);
}

void ColorAction::disconnectFromPublicAction()
{
    auto publicColorAction = dynamic_cast<ColorAction*>(getPublicAction());

    Q_ASSERT(publicColorAction != nullptr);

    disconnect(this, &ColorAction::colorChanged, publicColorAction, &ColorAction::setColor);
    disconnect(publicColorAction, &ColorAction::colorChanged, this, &ColorAction::setColor);

    WidgetAction::disconnectFromPublicAction();
}

WidgetAction* ColorAction::getPublicCopy() const
{
    return new ColorAction(parent(), text(), getColor(), getDefaultColor());
}

void ColorAction::fromVariantMap(const QVariantMap& variantMap)
{
    Serializable::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "Value");

    setColor(variantMap["Value"].value<QColor>());
}

QVariantMap ColorAction::toVariantMap() const
{
    QVariantMap variantMap = Serializable::toVariantMap();

    variantMap.insert({
        { "Value", QVariant::fromValue(_color) }
    });

    return variantMap;
}

ColorAction::PushButtonWidget::PushButtonWidget(QWidget* parent, ColorAction* colorAction) :
    WidgetActionWidget(parent, colorAction),
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
    layout->setSpacing(3);

    if (widgetFlags & WidgetFlag::Picker)
        layout->addWidget(new PushButtonWidget(parent, this));

    widget->setLayout(layout);

    return widget;
}

}
