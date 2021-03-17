#include "DropDataTypesWidget.h"
#include "Application.h"
#include "ScatterplotPlugin.h"
#include "ScatterplotWidget.h"

#include "PointData.h"
#include "ClusterData.h"
#include "ColorData.h"

#include <QHBoxLayout>
#include <QResizeEvent>
#include <QDragEnterEvent>
#include <QLabel>
#include <QVBoxLayout>
#include <QMimeData>

using namespace hdps;
using namespace hdps::gui;

DropDataTypesWidget::DropDataTypesWidget(QWidget* parent, ScatterplotPlugin* scatterplotPlugin) :
    QWidget(parent),
    _scatterplotPlugin(scatterplotPlugin),
    _getDropRegions(),
    _dragging(false)
{
    //setAcceptDrops(true);
    setMouseTracking(true);
    //setAttribute(Qt::WA_TransparentForMouseEvents, true);

    auto layout = new QHBoxLayout();

    layout->setMargin(0);
    layout->setSpacing(0);

    setLayout(layout);

    // Install event filter for synchronizing widget size
    parent->installEventFilter(this);

    // Install event filter for drag and drop
    //this->installEventFilter(this);
}

bool DropDataTypesWidget::eventFilter(QObject* target, QEvent* event)
{
    const auto isParentWidgetEvent  = dynamic_cast<QWidget*>(target) == parent();
    const auto targetWidget         = dynamic_cast<QWidget*>(target);
    

    switch (event->type())
    {
        case QEvent::Resize:
        {
            if (targetWidget != parent())
                break;

            setFixedSize(static_cast<QResizeEvent*>(event)->size());
            break;
        }

        case QEvent::DragEnter:
        {
            const auto dragEnterEvent = static_cast<QDragEnterEvent*>(event);

            if (isParentWidgetEvent) {
                qDebug() << "Parent drag enter";

                setAcceptDrops(false);

                removeAllDropRegionWidgets();

                auto dropRegions = _getDropRegions(dragEnterEvent->mimeData());

                for (auto dropRegion : dropRegions) {
                    const auto dropRegionWidget = new DropRegionWidget(dropRegion);
                    layout()->addWidget(dropRegionWidget);

                    dropRegionWidget->installEventFilter(this);
                }
            }

            break;
        }

        
        case QEvent::DragMove:
        {
            // Route drag and drop events to child region drop widgets
            if (isParentWidgetEvent) {
                qDebug() << "Parent drag move";
                
                //event->ignore();
                break;
            }

            break;
        }

        case QEvent::DragLeave:
        {
            if (isParentWidgetEvent) {
                qDebug() << "Parent drag leave";
                //hide();
                removeAllDropRegionWidgets();
            }

            auto dropRegionWidget = dynamic_cast<DropRegionWidget*>(target);

            if (dropRegionWidget) {

                setAcceptDrops(true);

                dropRegionWidget->setHighLight(false);

                removeAllDropRegionWidgets();
            }

            

            break;
        }

        case QEvent::Drop:
        {
            
            const auto dropEvent = static_cast<QDropEvent*>(event);

            if (isParentWidgetEvent) {
                qDebug() << "Parent drop";
                //dropEvent->ignore();
            }

            auto dropRegionWidget = dynamic_cast<DropRegionWidget*>(target);

            if (dropRegionWidget) {
                dropRegionWidget->getDropRegion()->drop();

                setAcceptDrops(true);

                removeAllDropRegionWidgets();

                dropEvent->acceptProposedAction();
            }

            break;
        }

        default:
            break;
    }

    /*
    auto mouseEvent = dynamic_cast<QMouseEvent*>(event);

    if (_dragging && mouseEvent)
        mouseEvent->ignore();
    */

    return QWidget::eventFilter(target, event);
}

void DropDataTypesWidget::initialize(GetDropRegions getDropRegions)
{
    _getDropRegions = getDropRegions;
}

void DropDataTypesWidget::removeAllDropRegionWidgets()
{
    QLayoutItem* child;

    while ((child = layout()->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }
}

DropDataTypesWidget::DropRegionWidget::DropRegionWidget(DropRegion* dropRegion, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _dropRegion(dropRegion),
    _labelsWidget(),
    _iconLabel(),
    _dataTypeLabel()
{
    setAcceptDrops(true);
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

    dropRegion->setParent(this);

    _dataTypeLabel.setText(_dropRegion->_title);

    auto mainLayout = new QVBoxLayout();

    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    setLayout(mainLayout);

    setHighLight(false);

    auto layout = new QVBoxLayout();

    layout->setMargin(0);
    layout->setSpacing(0);

    _labelsWidget.setObjectName("Labels");
    _labelsWidget.setLayout(layout);

    _iconLabel.setAlignment(Qt::AlignCenter);
    _dataTypeLabel.setAlignment(Qt::AlignCenter);

    const auto& fontAwesome = Application::getIconFont("FontAwesome");

    _iconLabel.setFont(fontAwesome.getFont(20));
    _iconLabel.setText(_dropRegion->_dropAllowed ? fontAwesome.getIconCharacter("plus-circle") : fontAwesome.getIconCharacter("exclamation-circle"));

    layout->addStretch(1);
    layout->addWidget(&_iconLabel);
    layout->addWidget(&_dataTypeLabel);
    layout->addStretch(1);

    mainLayout->addWidget(&_labelsWidget);
}

void DropDataTypesWidget::DropRegionWidget::setActive(const bool& active)
{
    _labelsWidget.setVisible(active);
}

void DropDataTypesWidget::DropRegionWidget::activate()
{
    setActive(true);
}

void DropDataTypesWidget::DropRegionWidget::deactivate()
{
    setActive(false);
}

void DropDataTypesWidget::DropRegionWidget::dragEnterEvent(QDragEnterEvent* dragEnterEvent)
{
    qDebug() << "DropRegionWidget::dragEnterEvent";

    dragEnterEvent->acceptProposedAction();

    setHighLight(true);
}

void DropDataTypesWidget::DropRegionWidget::dragLeaveEvent(QDragLeaveEvent* dragLeaveEvent)
{
    qDebug() << "DropRegionWidget::dragLeaveEvent";

    /*
    parentWidget()->setAcceptDrops(true);

    setHighLight(false);
    */
}

DropDataTypesWidget::DropRegion* DropDataTypesWidget::DropRegionWidget::getDropRegion()
{
    return _dropRegion;
}

void DropDataTypesWidget::DropRegionWidget::setHighLight(const bool& highlight /*= false*/)
{
    //qDebug() << _dataTypeLabel.text() << "setHighLight" << highlight;

    const auto backgroundColorString    = QString("rgba(%1, %2)").arg(_dropRegion->_dropAllowed ? "150, 150, 150" : "150, 0, 0", highlight ? "50" : "10");
    const auto borderString             = QString("%1 solid rgba(%2, %3)").arg(_dropRegion->_dropAllowed ? "0, 0, 0" : "150, 0, 0", highlight ? "1px" : "1px", highlight ? "50" : "10");

    _labelsWidget.setStyleSheet(QString("QWidget#Labels { background-color: %2; border: %3; }").arg(backgroundColorString, borderString));

    const auto textColorString = QString("rgba(%1, %2)").arg(_dropRegion->_dropAllowed ? "0, 0, 0" : "150, 0, 0", highlight ? "200" : "100");

    _iconLabel.setStyleSheet(QString("color: %1;").arg(textColorString));
    _dataTypeLabel.setStyleSheet(QString("color: %1;").arg(textColorString));
}

DropDataTypesWidget::DropRegion::DropRegion(QObject* parent, const QString& title, const bool& dropAllowed /*= true*/, const Dropped& dropped /*= Dropped()*/) :
    QObject(parent),
    _title(title),
    _dropAllowed(dropAllowed),
    _dropped(dropped)
{
}

void DropDataTypesWidget::DropRegion::drop()
{
    if (_dropped)
        _dropped();
}