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
    _hierarchyWidget(this, _model, &_filterModel)
{
    auto layout = new QVBoxLayout();

    auto toolbarLayout = new QHBoxLayout();

    toolbarLayout->addStretch(1);
    toolbarLayout->addWidget(_hierarchyWidget.getExpandAllAction().createWidget(this));
    toolbarLayout->addWidget(_hierarchyWidget.getCollapseAllAction().createWidget(this));

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(toolbarLayout);
    layout->addWidget(&_hierarchyWidget, 1);

    setLayout(layout);

    auto header = _hierarchyWidget.getTreeView().header();

    header->setStretchLastSection(false);

    //header->resizeSection(ActionHierarchyModelItem::Column::Name, 180);
    //header->resizeSection(ActionHierarchyModelItem::Column::Visible, header->minimumSectionSize());
    //header->resizeSection(ActionHierarchyModelItem::Column::Linkable, header->minimumSectionSize());

    header->setSectionResizeMode(ActionHierarchyModelItem::Column::Name, QHeaderView::Stretch);
    header->setSectionResizeMode(ActionHierarchyModelItem::Column::Visible, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(ActionHierarchyModelItem::Column::Linkable, QHeaderView::ResizeToContents);
}

}
}
