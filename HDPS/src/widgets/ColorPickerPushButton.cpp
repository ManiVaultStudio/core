#include "ColorPickerPushButton.h"

#include <QDebug>
#include <QPainter>

ColorPickerPushButton::ColorPickerPushButton(QWidget* parent) :
    QPushButton(parent),
    _colorDialog(this),
    _color(Qt::gray),
    _textMode(TextMode::NoText)
{
    QObject::connect(this, &QPushButton::clicked, [this]() {
        QObject::connect(&_colorDialog, &QColorDialog::currentColorChanged, [this](const QColor& color) {
            setColor(color);
        });

        _colorDialog.show();
    });
}

QColor ColorPickerPushButton::getColor() const
{
    return _color;
}

void ColorPickerPushButton::setColor(const QColor& color)
{
    if (color == _color)
        return;

    _color = color;

    _colorDialog.setCurrentColor(_color);

    update();

    emit colorChanged(_color);
}

ColorPickerPushButton::TextMode ColorPickerPushButton::getTextMode() const
{
    return _textMode;
}

void ColorPickerPushButton::setTextMode(const ColorPickerPushButton::TextMode& textMode)
{
    _textMode = textMode;

    update();
}

void ColorPickerPushButton::paintEvent(QPaintEvent* paintEvent)
{
    QPushButton::paintEvent(paintEvent);

    QPainter painter(this);

    //painter.setRenderHint(QPainter::Antialiasing);

    const auto margin = 5;

    QPoint offset(margin, margin);

    auto fillRect = rect();

    fillRect.setTopLeft(fillRect.topLeft() + offset);
    fillRect.setBottomRight(fillRect.bottomRight() - offset - QPoint(1, 1));

    auto color = getColor();

    if (!isEnabled()) {
        const auto grayScale = qGray(color.rgb());
        color.setRgb(grayScale, grayScale, grayScale);
    }

    painter.setBrush(QBrush(color));
    painter.setPen(QPen(QBrush(isEnabled() ? QColor(50, 50, 50) : Qt::gray), 1.0, Qt::SolidLine, Qt::SquareCap, Qt::SvgMiterJoin));
    painter.drawRect(fillRect);

    QString colorText;

    switch (_textMode)
    {
        case ColorPickerPushButton::TextMode::NoText:
            break;

        case ColorPickerPushButton::TextMode::RGB:
            colorText = QString("rgb(%1, %2, %3)").arg(QString::number(color.red()), QString::number(color.green()), QString::number(color.blue()));
            break;

        case ColorPickerPushButton::TextMode::Hexadecimal:
            colorText = color.name(QColor::HexRgb);
            break;

        default:
            break;
    }

    if (!colorText.isEmpty()) {
        QTextOption textOption;

        textOption.setAlignment(Qt::AlignCenter);

        painter.setFont(QFont("Arial", 7));
        painter.drawText(fillRect, colorText, textOption);
    }
}