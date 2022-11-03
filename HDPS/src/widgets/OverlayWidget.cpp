#include "OverlayWidget.h"

#include <QDebug>
#include <QResizeEvent>
#include <QVBoxLayout>

#include <stdexcept>

namespace hdps
{

namespace gui
{

OverlayWidget::OverlayWidget(QWidget* parent) :
    QWidget(parent),
    _opacityEffect(new QGraphicsOpacityEffect(this)),
    _iconLabel(),
    _titleLabel(),
    _descriptionLabel()
{
    initialize();
}

OverlayWidget::OverlayWidget(QWidget* parent, const QIcon& icon, const QString& title, const QString& description /*= ""*/) :
    QWidget(parent),
    _opacityEffect(new QGraphicsOpacityEffect(this)),
    _iconLabel(),
    _titleLabel(),
    _descriptionLabel()
{
    initialize();
}

void OverlayWidget::set(const QIcon& icon, const QString& title, const QString& description /*= ""*/)
{
    _iconLabel.setPixmap(icon.pixmap(QSize(24, 24)));
    _titleLabel.setText(title);
    _descriptionLabel.setText(description);
}

bool OverlayWidget::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Resize:
        {
            if (dynamic_cast<QWidget*>(target) != parent())
                break;

            setFixedSize(static_cast<QResizeEvent*>(event)->size());

            break;
        }

        default:
            break;
    }

    return QWidget::eventFilter(target, event);
}

void OverlayWidget::initialize()
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setGraphicsEffect(_opacityEffect);

    _opacityEffect->setOpacity(0.35);

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

    // Install event filter for synchronizing widget size
    parent()->installEventFilter(this);

    setObjectName("OverlayWidget");
    setStyleSheet("QWidget#OverlayWidget > QLabel { color: gray; }");
}

}
}
