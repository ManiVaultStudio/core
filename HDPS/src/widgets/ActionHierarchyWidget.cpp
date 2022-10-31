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
    _hierarchyWidget(this, "Action", _model, &_filterModel),
    _lastHoverModelIndex()
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
    _hierarchyWidget.getTreeView().setMouseTracking(true);

    connect(&_hierarchyWidget.getSelectionModel(), &QItemSelectionModel::currentChanged, this, [this](const QModelIndex& current, const QModelIndex& previous) -> void {
        setActionHighlighted(_hierarchyWidget.toSourceModelIndex(current.siblingAtColumn(ActionHierarchyModelItem::Column::Name)), true);
        setActionHighlighted(_hierarchyWidget.toSourceModelIndex(previous.siblingAtColumn(ActionHierarchyModelItem::Column::Name)), false);

        _lastHoverModelIndex = _hierarchyWidget.toSourceModelIndex(current.siblingAtColumn(ActionHierarchyModelItem::Column::Name));
    });
}

ActionHierarchyWidget::~ActionHierarchyWidget()
{
    if (_lastHoverModelIndex.isValid())
        setActionHighlighted(_lastHoverModelIndex, false);
}

void ActionHierarchyWidget::setActionHighlighted(const QModelIndex& index, bool highlighted)
{
    if (!index.isValid())
        return;

    if (index.internalPointer() == nullptr)
        return;

    auto actionHierarchyModelItem = static_cast<ActionHierarchyModelItem*>(index.internalPointer());

    actionHierarchyModelItem->getAction()->setHighlighted(highlighted);
}

}
}
