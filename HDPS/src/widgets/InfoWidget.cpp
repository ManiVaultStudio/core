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
    _iconLabel(),
    _titleLabel(),
    _descriptionLabel(),
    _backgroundColor(),
    _textColor()
{
    setColors(Qt::lightGray, Qt::black);
    initialize();
}

InfoWidget::InfoWidget(QWidget* parent, const QIcon& icon, const QString& title, const QString& description /*= ""*/, const QColor backgroundColor /*= Qt::lightgray*/, const QColor textColor /*= Qt::black*/) :
    QWidget(parent),
    _iconLabel(),
    _titleLabel(),
    _descriptionLabel(),
    _backgroundColor(),
    _textColor()
{
    set(icon, title, description);
    setColors(backgroundColor, textColor);
    initialize();
}

void InfoWidget::set(const QIcon& icon, const QString& title, const QString& description /*= ""*/)
{
    _iconLabel.setPixmap(icon.pixmap(QSize(24, 24)));
    _titleLabel.setText(title);
    _descriptionLabel.setText(description);

    //_titleLabel.setVisible(!_titleLabel.text().isEmpty());
    //_descriptionLabel.setVisible(!_descriptionLabel.text().isEmpty());
}

void InfoWidget::setColor(const QColor color)
{
    _backgroundColor    = color;
    _textColor          = color;

    _backgroundColor.setAlphaF(0.1f);

    setStyleSheet(QString("QWidget#InfoWidget { background-color: %1; } QWidget#InfoWidget > QLabel { color: %2; }").arg(_backgroundColor.name(), _textColor.name()));
}

void InfoWidget::setColors(const QColor backgroundColor, const QColor textColor)
{
    _backgroundColor    = backgroundColor;
    _textColor          = textColor;

    setStyleSheet(QString("QWidget#InfoWidget { background-color: %1; } QWidget#InfoWidget > QLabel { color: %2; }").arg(_backgroundColor.name(), _textColor.name()));
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

}
