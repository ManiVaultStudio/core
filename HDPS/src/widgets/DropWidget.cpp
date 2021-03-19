#include "DropWidget.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QResizeEvent>
#include <QDragEnterEvent>
#include <QLabel>
#include <QVBoxLayout>
#include <QMimeData>
#include <QIcon>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

namespace hdps
{

namespace gui
{

DropWidget::DropWidget(QWidget* parent) :
    QWidget(parent),
    _getDropRegionsFunction()
{
    setAcceptDrops(true);
    setMouseTracking(true);

    auto layout = new QHBoxLayout();

    layout->setMargin(0);
    layout->setSpacing(0);

    setLayout(layout);

    // Install event filter for drag and drop
    this->installEventFilter(this);

    // Install event filter for synchronizing widget size
    parent->installEventFilter(this);
}

bool DropWidget::eventFilter(QObject* target, QEvent* event)
{
    const auto isParentWidgetEvent  = dynamic_cast<QWidget*>(target) == parent();
    const auto isThisWidgetEvent    = dynamic_cast<QWidget*>(target) == this;

    switch (event->type())
    {
        case QEvent::Resize:
        {
            if (dynamic_cast<QWidget*>(target) != parent())
                break;

            setFixedSize(static_cast<QResizeEvent*>(event)->size());
            break;
        }

        case QEvent::DragEnter:
        {
            if (isThisWidgetEvent) {
                qDebug() << "Target widget drag enter event";

                const auto dragEnterEvent = static_cast<QDragEnterEvent*>(event);

                removeAllDropRegionWidgets();

                for (auto dropRegion : _getDropRegionsFunction(dragEnterEvent->mimeData()))
                    layout()->addWidget(new DropRegionContainerWidget(dropRegion, this));

                dragEnterEvent->acceptProposedAction();
            }

            break;
        }

        case QEvent::DragLeave:
        {
            if (isThisWidgetEvent) {
                qDebug() << "Target widget drag leave event";
                const auto dragLeaveEvent = static_cast<QDragLeaveEvent*>(event);
                removeAllDropRegionWidgets();
            }

            break;
        }

        case QEvent::Drop:
        {
            if (isThisWidgetEvent) {
                const auto dropEvent = static_cast<QDropEvent*>(event);

                removeAllDropRegionWidgets();
            }

            break;
        }

        default:
            break;
    }

    return QWidget::eventFilter(target, event);
}

void DropWidget::initialize(const GetDropRegionsFunction& getDropRegions)
{
    _getDropRegionsFunction = getDropRegions;
}

void DropWidget::removeAllDropRegionWidgets()
{
    QLayoutItem* child;

    while ((child = layout()->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }
}

DropWidget::DropRegionContainerWidget::DropRegionContainerWidget(DropRegion* dropRegion, QWidget* parent) :
    QWidget(parent),
    _dropRegion(dropRegion),
    _opacityEffect(new QGraphicsOpacityEffect(this)),
    _opacityAnimation(new QPropertyAnimation(_opacityEffect, "opacity"))
{
    setAutoFillBackground(true);
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    setGraphicsEffect(_opacityEffect);

    _opacityAnimation->setDuration(200);

    dropRegion->setParent(this);

    auto mainLayout = new QVBoxLayout();

    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(_dropRegion->getWidget());

    setLayout(mainLayout);

    setHighLight(false);

    // Respond to drag move events of the parent widget so that we can highlight when needed
    parent->installEventFilter(this);
}

DropWidget::DropRegion* DropWidget::DropRegionContainerWidget::getDropRegion()
{
    return _dropRegion;
}

bool DropWidget::DropRegionContainerWidget::eventFilter(QObject* target, QEvent* event)
{
    const auto isParentWidgetEvent = dynamic_cast<QWidget*>(target) == parent();

    qDebug() << event;
    switch (event->type())
    {
        case QEvent::DragMove:
        {
            if (isParentWidgetEvent) {
                qDebug() << "isParentWidgetEvent";
                const auto dragMoveEvent = static_cast<QDragMoveEvent*>(event);
                setHighLight(rect().contains(dragMoveEvent->pos()));
            }

            break;
        }

        case QEvent::Drop:
        {
            const auto dropEvent = static_cast<QDropEvent*>(event);

            if (rect().contains(dropEvent->pos()))
                getDropRegion()->drop();

            break;
        }

        default:
            break;
    }

    return QWidget::eventFilter(target, event);
}

void DropWidget::DropRegionContainerWidget::setHighLight(const bool& highlight /*= false*/)
{
    const auto targetOpacity = highlight ? 0.9 : 0.6;

    if (_opacityEffect->opacity() != targetOpacity)
        _opacityEffect->setOpacity(targetOpacity);

    /*
    _opacityAnimation->setStartValue(_opacityEffect->opacity());
    _opacityAnimation->setEndValue(targetOpacity);
    _opacityAnimation->start();
    */
}

DropWidget::DropRegion::DropRegion(QObject* parent, QWidget* widget, const Dropped& dropped /*= Dropped()*/) :
    QObject(parent),
    _widget(widget),
    _dropped(dropped)
{
}

DropWidget::DropRegion::DropRegion(QObject* parent, const QIcon& icon, const QString& title, const QString& description, const bool& dropAllowed /*= true*/, const Dropped& dropped /*= Dropped()*/) :
    QObject(parent),
    _widget(new StandardWidget(nullptr, icon, title, description, dropAllowed)),
    _dropped(dropped)
{
}

QWidget* DropWidget::DropRegion::getWidget() const
{
    Q_ASSERT(_widget != nullptr);

    return _widget;
}

void DropWidget::DropRegion::drop()
{
    if (_dropped)
        _dropped();
}

DropWidget::DropRegion::StandardWidget::StandardWidget(QWidget* parent, const QIcon& icon, const QString& title, const QString& description, const bool& dropAllowed /*= true*/) :
    QWidget(parent)
{
    setObjectName("StandardWidget");

    auto layout = new QVBoxLayout();

    auto iconLabel          = new QLabel();
    auto titleLabel         = new QLabel(title);
    auto descriptionLabel   = new QLabel(description);

    iconLabel->setPixmap(icon.pixmap(QSize(32, 32)));

    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-weight: bold");

    descriptionLabel->setAlignment(Qt::AlignCenter);

    layout->setMargin(0);
    layout->setSpacing(0);
    layout->setAlignment(Qt::AlignCenter);

    layout->addStretch(1);
    layout->addWidget(iconLabel);
    layout->addWidget(titleLabel);
    layout->addWidget(descriptionLabel);
    layout->addStretch(1);

    setLayout(layout);

    const auto saturation       = dropAllowed ? 0 : 100;
    const auto backgroundColor  = QString("hsl(0, %1%, 90%)").arg(QString::number(saturation));
    const auto foregroundColor  = QString("hsl(0, %1%, 30%)").arg(QString::number(saturation));
    const auto borderColor      = QString("hsl(0, %1%, 60%)").arg(QString::number(saturation));
    const auto border           = QString("2px solid %1").arg(borderColor);

    setStyleSheet(QString("QWidget#StandardWidget{ background-color: %1; border: %2; } QLabel { color: %3; }").arg(backgroundColor, border, foregroundColor));
}

}
}