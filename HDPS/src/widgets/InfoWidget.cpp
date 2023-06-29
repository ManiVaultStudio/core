#include "InfoWidget.h"

#include <QDebug>
#include <QVBoxLayout>

#ifdef _DEBUG
    #define INFO_WIDGET_VERBOSE
#endif

namespace hdps::gui
{

InfoWidget::InfoWidget(QWidget* parent) :
    QWidget(parent),
    _icon(),
    _iconLabel(),
    _titleLabel(),
    _descriptionLabel(),
    _foregroundColor(),
    _backgroundColor()
{
    setColors(Qt::black, Qt::lightGray);
    initialize();
}

InfoWidget::InfoWidget(QWidget* parent, const QIcon& icon, const QString& title, const QString& description /*= ""*/, const QColor foregroundColor /*= Qt::black*/, const QColor backgroundColor /*= Qt::lightGray*/) :
    QWidget(parent),
    _icon(),
    _iconLabel(),
    _titleLabel(),
    _descriptionLabel(),
    _foregroundColor(),
    _backgroundColor()
{
    set(icon, title, description);
    setColors(foregroundColor, backgroundColor);
    initialize();
}

void InfoWidget::set(const QIcon& icon, const QString& title, const QString& description /*= ""*/)
{
    _icon = icon;

    _iconLabel.setPixmap(icon.pixmap(QSize(24, 24)));
    _titleLabel.setText(title);
    _descriptionLabel.setText(description);

    _titleLabel.setVisible(!_titleLabel.text().isEmpty());
    _descriptionLabel.setVisible(!_descriptionLabel.text().isEmpty());
}

void InfoWidget::setColor(const QColor color)
{
    _foregroundColor    = color;
    _backgroundColor    = color;
    
    _backgroundColor.setAlphaF(0.1f);

    setColors(_foregroundColor, _backgroundColor);
}

void InfoWidget::setColors(const QColor foregroundColor /*= Qt::black*/, const QColor backgroundColor /*= Qt::lightGray*/)
{
    _foregroundColor    = foregroundColor;
    _backgroundColor    = backgroundColor;

    const auto colorToRgba = [](const QColor& color) -> QString {
        return QString("rgba(%1,%2,%3,%4)").arg(QString::number(color.red()), QString::number(color.green()), QString::number(color.blue()), QString::number(color.alpha()));
    };

    setStyleSheet(QString("QWidget#InfoWidget { background-color: %1; } QWidget#InfoWidget > QLabel { color: %2; }").arg(colorToRgba(_backgroundColor), colorToRgba(_foregroundColor)));

    QImage tmp = _icon.pixmap(QSize(24, 24)).toImage();

    for (int pixelY = 0; pixelY < tmp.height(); pixelY++)
    {
        for (int pixelX = 0; pixelX < tmp.width(); pixelX++)
        {
            auto color = _foregroundColor;

            color.setAlpha(_foregroundColor.alphaF() * tmp.pixelColor(pixelX, pixelY).alpha());

            tmp.setPixelColor(pixelX, pixelY, color);
        }
    }

    _iconLabel.setPixmap(QPixmap::fromImage(tmp));
}

void InfoWidget::initialize()
{
    setObjectName("InfoWidget");

    auto layout = new QVBoxLayout();

    _iconLabel.setAlignment(Qt::AlignCenter);

    _titleLabel.setAlignment(Qt::AlignCenter);
    _titleLabel.setStyleSheet("font-weight: bold");

    _descriptionLabel.setAlignment(Qt::AlignCenter);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setAlignment(Qt::AlignCenter);

    layout->addStretch(1);
    layout->addWidget(&_iconLabel);
    layout->addWidget(&_titleLabel);
    layout->addWidget(&_descriptionLabel);
    layout->addStretch(1);

    setLayout(layout);
}

void InfoWidget::setForegroundColor(const QColor foregroundColor /*= Qt::black*/)
{
    _foregroundColor = foregroundColor;

    setColors(_foregroundColor, _backgroundColor);
}

void InfoWidget::setBackgroundColor(const QColor backgroundColor /*= Qt::lightGray*/)
{
    _backgroundColor = backgroundColor;

    setColors(_foregroundColor, _backgroundColor);
}

}
