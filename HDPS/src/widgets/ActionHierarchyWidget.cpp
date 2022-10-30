#include "ActionHierarchyWidget.h"
#include "ActionHierarchyModelItem.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>

using namespace hdps::util;

namespace hdps
{

namespace gui
{

ActionHierarchyWidget::ActionHierarchyWidget(QWidget* parent) :
    QWidget(parent),
    _model(this),
    _filterModel(this),
    _hierarchyWidget(this, "Action", _model, &_filterModel)
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(&_hierarchyWidget, 1);

    setLayout(layout);

    auto header = _hierarchyWidget.getTreeView().header();

    header->setStretchLastSection(false);

    header->setSectionResizeMode(ActionHierarchyModelItem::Column::Name, QHeaderView::Stretch);
    header->setSectionResizeMode(ActionHierarchyModelItem::Column::Visible, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(ActionHierarchyModelItem::Column::Linkable, QHeaderView::ResizeToContents);
}

}
}
