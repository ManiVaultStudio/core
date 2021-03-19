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

    auto layout = new QHBoxLayout();

    layout->setMargin(0);
    layout->setSpacing(0);

    setLayout(layout);

    // Install event filter for synchronizing widget size
    parent->installEventFilter(this);
}

bool DropWidget::eventFilter(QObject* target, QEvent* event)
{
    const auto isParentWidgetEvent = dynamic_cast<QWidget*>(target) == parent();

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
            const auto dragEnterEvent = static_cast<QDragEnterEvent*>(event);

            if (isParentWidgetEvent) {
                //setAcceptDrops(false);
                removeAllDropRegionWidgets();

                for (auto dropRegion : _getDropRegionsFunction(dragEnterEvent->mimeData())) {
                    const auto dropRegionContainerWidget = new DropRegionContainerWidget(dropRegion);

                    layout()->addWidget(dropRegionContainerWidget);
                    dropRegionContainerWidget->installEventFilter(this);
                }
            }

            auto dropRegionContainerWidget = dynamic_cast<DropRegionContainerWidget*>(target);

            if (dropRegionContainerWidget) {
                dragEnterEvent->acceptProposedAction();
                dropRegionContainerWidget->setHighLight(true);
            }

            break;
        }

        case QEvent::DragLeave:
        {
            if (isParentWidgetEvent) {
                qDebug() << "removeAllDropRegionWidgets";
                removeAllDropRegionWidgets();
            }

            auto dropRegionContainerWidget = dynamic_cast<DropRegionContainerWidget*>(target);

            if (dropRegionContainerWidget) {
                //setAcceptDrops(true);
                dropRegionContainerWidget->setHighLight(false);
                //removeAllDropRegionWidgets();
                event->accept();
            }

            break;
        }

        case QEvent::Drop:
        {
            const auto dropEvent = static_cast<QDropEvent*>(event);

            auto dropRegionContainerWidget = dynamic_cast<DropRegionContainerWidget*>(target);

            if (dropRegionContainerWidget) {
                dropRegionContainerWidget->getDropRegion()->drop();

                //setAcceptDrops(true);

                removeAllDropRegionWidgets();

                dropEvent->acceptProposedAction();
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

DropWidget::DropRegionContainerWidget::DropRegionContainerWidget(DropRegion* dropRegion, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _dropRegion(dropRegion),
    _opacityEffect(new QGraphicsOpacityEffect(this)),
    _opacityAnimation(new QPropertyAnimation(_opacityEffect, "opacity"))
{
    setAcceptDrops(true);
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
}

DropWidget::DropRegion* DropWidget::DropRegionContainerWidget::getDropRegion()
{
    return _dropRegion;
}

void DropWidget::DropRegionContainerWidget::setHighLight(const bool& highlight /*= false*/)
{
    const auto targetOpacity = highlight ? 0.9 : 0.6;

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