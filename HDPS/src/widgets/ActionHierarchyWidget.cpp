#include "ActionHierarchyWidget.h"
#include "ActionHierarchyModelItem.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QHeaderView>

using namespace hdps::util;

namespace hdps
{

namespace gui
{

ActionHierarchyWidget::ActionHierarchyWidget(QWidget* parent, WidgetAction* rootAction) :
    QWidget(parent),
    _model(this, rootAction),
    _filterModel(this),
    _hierarchyWidget(this, "Action", _model, &_filterModel)
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(&_hierarchyWidget, 1);

    setLayout(layout);

    auto header = _hierarchyWidget.getTreeView().header();

    header->setStretchLastSection(false);

    const auto checkColumnSize = 55;

    header->resizeSection(ActionHierarchyModelItem::Column::Visible, checkColumnSize);
    header->resizeSection(ActionHierarchyModelItem::Column::Enabled, checkColumnSize);
    header->resizeSection(ActionHierarchyModelItem::Column::Linkable, checkColumnSize);

    header->setSectionResizeMode(ActionHierarchyModelItem::Column::Name, QHeaderView::Stretch);
    header->setSectionResizeMode(ActionHierarchyModelItem::Column::Visible, QHeaderView::Fixed);
    header->setSectionResizeMode(ActionHierarchyModelItem::Column::Enabled, QHeaderView::Fixed);
    header->setSectionResizeMode(ActionHierarchyModelItem::Column::Linkable, QHeaderView::Fixed);

    _hierarchyWidget.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("play"));
}

}
}
