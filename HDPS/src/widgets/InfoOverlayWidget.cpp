#include "InfoOverlayWidget.h"

#include <QDebug>
#include <QVBoxLayout>

#ifdef _DEBUG
    #define INFO_OVERLAY_WIDGET_VERBOSE
#endif

namespace hdps::gui
{

InfoOverlayWidget::InfoOverlayWidget(QWidget* parent) :
    OverlayWidget(parent),
    _iconLabel(),
    _titleLabel(),
    _descriptionLabel(),
    _backgroundColor(),
    _textColor()
{
    setColors(Qt::lightGray, Qt::black);
    initialize();
}

InfoOverlayWidget::InfoOverlayWidget(QWidget* parent, const QIcon& icon, const QString& title, const QString& description /*= ""*/, const QColor backgroundColor /*= Qt::lightgray*/, const QColor textColor /*= Qt::black*/) :
    OverlayWidget(parent),
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

void InfoOverlayWidget::set(const QIcon& icon, const QString& title, const QString& description /*= ""*/)
{
    _iconLabel.setPixmap(icon.pixmap(QSize(24, 24)));
    _titleLabel.setText(title);
    _descriptionLabel.setText(description);

    //_titleLabel.setVisible(!_titleLabel.text().isEmpty());
    //_descriptionLabel.setVisible(!_descriptionLabel.text().isEmpty());
}

void InfoOverlayWidget::setColor(const QColor color)
{
    _backgroundColor    = color;
    _textColor          = color;

    _backgroundColor.setAlphaF(0.1f);

    setStyleSheet(QString("QWidget#InfoOverlayWidget { background-color: %1; } QWidget#InfoOverlayWidget > QLabel { color: %2; }").arg(_backgroundColor.name(), _textColor.name()));
}

void InfoOverlayWidget::setColors(const QColor backgroundColor, const QColor textColor)
{
    _backgroundColor    = backgroundColor;
    _textColor          = textColor;

    setStyleSheet(QString("QWidget#InfoOverlayWidget { background-color: %1; } QWidget#InfoOverlayWidget > QLabel { color: %2; }").arg(_backgroundColor.name(), _textColor.name()));
}

void InfoOverlayWidget::initialize()
{
    OverlayWidget::initialize();

    setAttribute(Qt::WA_TransparentForMouseEvents);

    getWidgetFader().setMaximumOpacity(0.35f);

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

    setObjectName("InfoOverlayWidget");
}

}
