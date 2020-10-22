#include "ColorPickerPushButton.h"

#include <QDebug>
#include <QColorDialog>
#include <QPainter>

const QSize ColorPickerPushButton::_iconSize = QSize(12, 12);

ColorPickerPushButton::ColorPickerPushButton(QWidget* parent) :
	QPushButton(parent),
	_colorDialog(new QColorDialog(parent)),
    _showText(true)
{
	setIconSize(_iconSize);
	this->setStyleSheet("text-align:left; margin: -1px; padding-left: 5px; padding-top: 4px; padding-bottom: 3px;");

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

	auto icon = QPixmap(_iconSize);

	icon.fill(color);

	auto painter = new QPainter(&icon);

	painter->setPen(QPen(QBrush(QColor(30, 30, 30)), 1.5f));

	QPointF points[5] = {
		QPointF(0.0f, 0.0f),
		QPointF(_iconSize.width(), 0.0f),
		QPointF(_iconSize.width(), _iconSize.height()),
		QPointF(0.0f, _iconSize.height()),
		QPointF(0.0f, 0.0f)
	};

	painter->drawPolyline(points, 5);

	setIcon(icon);
		
	delete painter;
	
    if (_showText) {
        const auto colorString = QString("rgb(%1, %2, %3)").arg(QString::number(color.red()), QString::number(color.green()), QString::number(color.blue()));

        setText(colorString);
    }

	emit colorChanged(color);
}

bool ColorPickerPushButton::getShowText() const
{
    return _showText;
}

void ColorPickerPushButton::setShowText(const bool& showText)
{
    _showText = showText;

    setColor(_colorDialog->currentColor());
}