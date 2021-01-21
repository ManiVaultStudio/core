#include "PopupPushButton.h"
#include "PopupWidget.h"

#include <QEvent>
#include <QPainter>
#include <QVariant>

namespace hdps
{

namespace gui
{

PopupPushButton::PopupPushButton() :
    QPushButton(),
    _widget(nullptr),
    _alignment(PopupAlignmentFlag::BottomLeft),
    _popupWidget(new QWidget()),
    _popupLayout(new QVBoxLayout()),
    _groupBox(new QGroupBox()),
    _groupBoxLayout(new QVBoxLayout())
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    setProperty("cssClass", "popup");

    _popupWidget->installEventFilter(this);
    _popupWidget->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    _popupWidget->setObjectName("PopupWidget");
    _popupWidget->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
    _popupWidget->setLayout(_popupLayout);

    _popupLayout->setMargin(7);
    _popupLayout->addWidget(_groupBox);

    _groupBox->setLayout(_groupBoxLayout);

    connect(this, &QPushButton::clicked, [this]() {
        _groupBoxLayout->takeAt(0);
        _groupBoxLayout->addWidget(_widget);

        _widget->show();
        _popupWidget->show();

        QPoint move;

        if (_alignment.testFlag(PopupAlignmentFlag::DockTop) || _alignment.testFlag(PopupAlignmentFlag::DockBottom)) {
            if (_alignment.testFlag(PopupAlignmentFlag::DockTop))
                move.setY(mapToGlobal(rect().topLeft()).y() - _popupWidget->height() - 1);

            if (_alignment.testFlag(PopupAlignmentFlag::DockBottom))
                move.setY(mapToGlobal(rect().bottomRight()).y() + 1);

            if (_alignment.testFlag(PopupAlignmentFlag::AnchorLeft))
                move.setX(mapToGlobal(rect().topLeft()).x() + 1);

            if (_alignment.testFlag(PopupAlignmentFlag::AnchorCenter))
                move.setX(mapToGlobal(rect().center()).x() - _popupWidget->rect().center().x());

            if (_alignment.testFlag(PopupAlignmentFlag::AnchorRight))
                move.setX(mapToGlobal(rect().topRight()).x() - _popupWidget->width());
        }

        if (_alignment.testFlag(PopupAlignmentFlag::DockLeft) || _alignment.testFlag(PopupAlignmentFlag::DockRight)) {
            if (_alignment.testFlag(PopupAlignmentFlag::DockLeft))
                move.setX(mapToGlobal(rect().topLeft()).x() - _popupWidget->width());

            if (_alignment.testFlag(PopupAlignmentFlag::DockRight))
                move.setX(mapToGlobal(rect().bottomRight()).x() + 1);

            if (_alignment.testFlag(PopupAlignmentFlag::AnchorTop))
                move.setY(mapToGlobal(rect().topLeft()).y() + 1);

            if (_alignment.testFlag(PopupAlignmentFlag::AnchorCenter))
                move.setY(mapToGlobal(rect().center()).y() - _popupWidget->rect().center().y());

            if (_alignment.testFlag(PopupAlignmentFlag::AnchorBottom))
                move.setY(mapToGlobal(rect().bottomLeft()).y() - _popupWidget->height());
        }

        _popupWidget->move(move);
    });
}

void PopupPushButton::setWidget(QWidget* widget)
{
    Q_ASSERT(widget != nullptr);

    _widget = widget;

    _groupBox->setTitle(_widget->windowTitle());

    connect(_widget, &QWidget::windowTitleChanged, [this]() {
        _groupBox->setTitle(_widget->windowTitle());
        setToolTip(QString("Click to edit %1 settings").arg(_widget->windowTitle().toLower()));
    });
}

bool PopupPushButton::eventFilter(QObject* object, QEvent* event)
{
    const auto widget = dynamic_cast<QWidget*>(object);

    if (widget != _popupWidget)
        return QObject::eventFilter(object, event);

    switch (event->type())
    {
        case QEvent::FocusOut:
        case QEvent::Close:
            _groupBoxLayout->takeAt(0);
            emit popupClosed();
            break;

        default:
            break;
    }

    return QObject::eventFilter(object, event);
}

void PopupPushButton::paintEvent(QPaintEvent* paintEvent)
{
    QPushButton::paintEvent(paintEvent);

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    const auto size = 3.0f;
    const auto bottomRight = QPointF(rect().bottomRight()) - QPointF(2.0f, 2.0f);
    const auto topLeft = bottomRight - QPointF(size, size);

    const QVector<QPointF> points{ topLeft, topLeft + QPointF(size, 0.0f), bottomRight - QPointF(size / 2.0f, 0.0f) };

    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(isEnabled() ? Qt::black : Qt::gray));
    painter.drawPolygon(points.constData(), points.count());
}

PopupPushButton::PopupAlignment PopupPushButton::getPopupAlignment() const
{
    return _alignment;
}

void PopupPushButton::setPopupAlignment(const PopupAlignment& alignment)
{
    if (alignment == _alignment)
        return;

    _alignment = alignment;
}

}
}