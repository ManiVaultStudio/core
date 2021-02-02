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
    setProperty("cssClass", "square");

    _popupWidget->installEventFilter(this);
    _popupWidget->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    _popupWidget->setObjectName("PopupWidget");
    _popupWidget->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum));
    _popupWidget->setLayout(_popupLayout);

    _popupLayout->setMargin(7);
    _popupLayout->addWidget(_groupBox);
    _popupLayout->setSizeConstraint(QLayout::SetFixedSize);

    _groupBox->setLayout(_groupBoxLayout);
    _groupBox->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum));

    connect(this, &QPushButton::clicked, [this]() {
        Q_ASSERT(_widget != nullptr);

        _groupBoxLayout->takeAt(0);
        _groupBoxLayout->addWidget(_widget);

        emit popupOpen();

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
            emit popupClose();
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

    const auto margin = 5.0f;

    QPointF center;

    if (_alignment.testFlag(PopupAlignmentFlag::DockTop) || _alignment.testFlag(PopupAlignmentFlag::DockBottom)) {
        if (_alignment.testFlag(PopupAlignmentFlag::DockTop))
            center.setY(margin);

        if (_alignment.testFlag(PopupAlignmentFlag::DockBottom))
            center.setY(height() - margin);

        if (_alignment.testFlag(PopupAlignmentFlag::AnchorLeft))
            center.setX(margin);

        if (_alignment.testFlag(PopupAlignmentFlag::AnchorCenter))
            center.setX(rect().center().x());

        if (_alignment.testFlag(PopupAlignmentFlag::AnchorRight))
            center.setX(width() - margin);
    }

    if (_alignment.testFlag(PopupAlignmentFlag::DockLeft) || _alignment.testFlag(PopupAlignmentFlag::DockRight)) {
        if (_alignment.testFlag(PopupAlignmentFlag::DockLeft))
            center.setX(margin);

        if (_alignment.testFlag(PopupAlignmentFlag::DockRight))
            center.setX(width() - margin);

        if (_alignment.testFlag(PopupAlignmentFlag::AnchorTop))
            center.setY(margin);

        if (_alignment.testFlag(PopupAlignmentFlag::AnchorCenter))
            center.setY(rect().center().y());

        if (_alignment.testFlag(PopupAlignmentFlag::AnchorBottom))
            center.setY(height() - margin);
    }

    painter.setPen(QPen(QBrush(isEnabled() ? Qt::black : Qt::gray), 2.5, Qt::SolidLine, Qt::RoundCap));
    painter.setBrush(Qt::NoBrush);
    painter.drawPoint(center);
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