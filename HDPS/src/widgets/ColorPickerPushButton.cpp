#include "ColorPickerPushButton.h"

#include <QDebug>
#include <QColorDialog>
#include <QPainter>

ColorPickerPushButton::ColorPickerPushButton(QWidget* parent) :
	QPushButton(parent),
	_colorDialog(new QColorDialog(parent)),
    _showText(true)
{
    QObject::connect(this, &QPushButton::clicked, [this]() {
        QObject::connect(_colorDialog, &QColorDialog::currentColorChanged, [this](const QColor& color) {
            setColor(color);
        });

        _colorDialog->show();
    });
}

QColor ColorPickerPushButton::getColor() const
{
	return _colorDialog->currentColor();
}

void ColorPickerPushButton::setColor(const QColor& color)
{
    if (color != getColor())
        _colorDialog->setCurrentColor(color);

    update();

    emit colorChanged(color);
}

bool ColorPickerPushButton::getShowText() const
{
    return _showText;
}

void ColorPickerPushButton::setShowText(const bool& showText)
{
    _showText = showText;

    update();
}

void ColorPickerPushButton::paintEvent(QPaintEvent* paintEvent)
{
    QPushButton::paintEvent(paintEvent);

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    const auto color = getColor();

    const auto margin = 5;

    QPoint offset(margin, margin);

    auto fillRect = rect();

    fillRect.setTopLeft(fillRect.topLeft() + offset);
    fillRect.setBottomRight(fillRect.bottomRight() - offset);

    painter.setBrush(QBrush(color));
    painter.setPen(QPen(QBrush(Qt::black), 0.5, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    painter.drawRect(fillRect);

    if (_showText) {
        QTextOption textOption;

        textOption.setAlignment(Qt::AlignCenter);

        painter.setFont(QFont("Arial", 7));
        painter.drawText(fillRect, QString("rgb(%1, %2, %3)").arg(QString::number(color.red()), QString::number(color.green()), QString::number(color.blue())), textOption);
    }
}