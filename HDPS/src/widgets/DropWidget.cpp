#include "DropWidget.h"

#include <QHBoxLayout>
#include <QResizeEvent>
#include <QDragEnterEvent>
#include <QLabel>
#include <QVBoxLayout>
#include <QMimeData>

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
    _dropRegion(dropRegion)
{
    setAcceptDrops(true);
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

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
}

DropWidget::DropRegion::DropRegion(QObject* parent, QWidget* widget, const Dropped& dropped /*= Dropped()*/) :
    QObject(parent),
    _widget(widget),
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

}
}