#include "DropWidget.h"
#include "Application.h"

#include <QApplication>
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
    _getDropRegionsFunction(),
    _showDropIndicator(true),
    _dropIndicatorWidget(nullptr)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    
    auto layout = new QHBoxLayout();

    layout->setMargin(0);
    layout->setSpacing(0);

    setLayout(layout);

    // Install event filter for synchronizing widget size
    parent->installEventFilter(this);
}

bool DropWidget::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Resize:
        {
            if (dynamic_cast<QWidget*>(target) != parent())
                break;

            const auto parentWidgetSize = static_cast<QResizeEvent*>(event)->size();

            setFixedSize(parentWidgetSize);

            if (_dropIndicatorWidget)
                _dropIndicatorWidget->setFixedSize(parentWidgetSize);

            break;
        }

        case QEvent::DragEnter:
        {
            if (dynamic_cast<QWidget*>(target) != parent())
                break;

            const auto dragEnterEvent = static_cast<QDragEnterEvent*>(event);

            resetLayout();

            for (auto dropRegion : _getDropRegionsFunction(dragEnterEvent->mimeData()))
                layout()->addWidget(new DropRegionContainerWidget(dropRegion, this));

            dragEnterEvent->acceptProposedAction();
            
            if (_dropIndicatorWidget)
                _dropIndicatorWidget->hide();

            break;
        }

        case QEvent::DragMove:
        {
            if (dynamic_cast<QWidget*>(target) != parent())
                break;

            const auto dragMoveEvent = static_cast<QDragMoveEvent*>(event);

            if (layout()->count() > 1) {
                for (int i = 0; i < layout()->count(); ++i)
                {
                    auto dropRegionContainerWidget = dynamic_cast<DropRegionContainerWidget*>(layout()->itemAt(i)->widget());

                    if (dropRegionContainerWidget)
                        dropRegionContainerWidget->setHighLight(dropRegionContainerWidget->geometry().contains(dragMoveEvent->pos()));
                }
            }

            break;
        }
        
        case QEvent::DragLeave:
        {
            if (dynamic_cast<QWidget*>(target) != parent())
                break;

            resetLayout();

            if (_dropIndicatorWidget)
                _dropIndicatorWidget->setVisible(_showDropIndicator);

            break;
        }

        case QEvent::Drop:
        {
            if (dynamic_cast<QWidget*>(target) != parent())
                break;

            const auto dropEvent = static_cast<QDropEvent*>(event);

            for (int i = 0; i < layout()->count(); ++i)
            {
                auto dropRegionContainerWidget = dynamic_cast<DropRegionContainerWidget*>(layout()->itemAt(i)->widget());

                if (dropRegionContainerWidget) {
                    if (dropRegionContainerWidget->geometry().contains(dropEvent->pos()))
                        dropRegionContainerWidget->getDropRegion()->drop();
                }
            }

            resetLayout();

            if (_dropIndicatorWidget)
                _dropIndicatorWidget->setVisible(_showDropIndicator);

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

bool DropWidget::getShowDropIndicator() const
{
    return _showDropIndicator;
}

void DropWidget::setShowDropIndicator(const bool& showDropIndicator)
{
    if (showDropIndicator == _showDropIndicator)
        return;

    _showDropIndicator = showDropIndicator;

    if (_dropIndicatorWidget)
        _dropIndicatorWidget->setVisible(showDropIndicator);
}

void DropWidget::setDropIndicatorWidget(QWidget* dropIndicatorWidget)
{
    Q_ASSERT(dropIndicatorWidget != nullptr);

    _dropIndicatorWidget = dropIndicatorWidget;

    _dropIndicatorWidget->setParent(parentWidget());
}

void DropWidget::resetLayout()
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
}

DropWidget::DropRegion* DropWidget::DropRegionContainerWidget::getDropRegion()
{
    return _dropRegion;
}

void DropWidget::DropRegionContainerWidget::setHighLight(const bool& highlight /*= false*/)
{
    const auto targetOpacity = highlight ? 0.9 : 0.5;

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

DropWidget::DropRegion::DropRegion(QObject* parent, const QString& title, const QString& description, const bool& dropAllowed /*= true*/, const Dropped& dropped /*= Dropped()*/) :
    QObject(parent),
    _widget(new StandardWidget(nullptr, title, description, dropAllowed)),
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

DropWidget::DropRegion::StandardWidget::StandardWidget(QWidget* parent, const QString& title, const QString& description, const bool& dropAllowed /*= true*/) :
    QWidget(parent)
{
    auto layout = new QVBoxLayout();

    auto iconLabel          = new QLabel();
    auto titleLabel         = new QLabel(title);
    auto descriptionLabel   = new QLabel(description);

    const auto& fontAwesome = Application::getIconFont("FontAwesome");

    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setFont(fontAwesome.getFont(14));
    iconLabel->setText(fontAwesome.getIconCharacter(dropAllowed ? "file-import" : "exclamation-circle"));
    iconLabel->setStyleSheet("QLabel { padding: 10px; }");

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
    const auto backgroundColor  = QString("hsl(0, %1%, 97%)").arg(QString::number(saturation));
    const auto foregroundColor  = QString("hsl(0, %1%, 30%)").arg(QString::number(saturation));
    const auto borderColor      = QString("hsl(0, %1%, 50%)").arg(QString::number(saturation));
    const auto border           = QString("1px solid %1").arg(borderColor);

    setObjectName("StandardWidget");
    setStyleSheet(QString("QWidget#StandardWidget{ background-color: %1; border: %2; } QLabel { color: %3; }").arg(backgroundColor, border, foregroundColor));
}

DropWidget::DropIndicatorWidget::DropIndicatorWidget(QWidget* parent, const QString& title, const QString& description) :
    QWidget(parent),
    _opacityEffect(new QGraphicsOpacityEffect(this))
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setGraphicsEffect(_opacityEffect);

    _opacityEffect->setOpacity(0.35);

    auto layout = new QVBoxLayout();

    auto iconLabel          = new QLabel();
    auto titleLabel         = new QLabel(title);
    auto descriptionLabel   = new QLabel(description);

    const auto& fontAwesome = Application::getIconFont("FontAwesome");

    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setFont(fontAwesome.getFont(14));
    iconLabel->setText(fontAwesome.getIconCharacter("file-import"));
    iconLabel->setStyleSheet("QLabel { padding: 10px; }");

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

    setObjectName("DropIndicatorWidget");
    setStyleSheet("QWidget#DropIndicatorWidget > QLabel { color: gray; } QWidget#DropIndicatorWidget { background-color: hsl(0, 0%, 97%); }");
}

}
}