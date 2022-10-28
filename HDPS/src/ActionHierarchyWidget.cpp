#include "ActionHierarchyWidget.h"
#include "ActionHierarchyModel.h"
#include "ActionHierarchyModelItem.h"

#include <QDebug>
#include <QHeaderView>
#include <QVBoxLayout>

#include <stdexcept>

using namespace hdps::util;

namespace hdps
{

namespace gui
{

ActionHierarchyWidget::ActionHierarchyWidget(QWidget* parent) :
    QWidget(parent),
    _model(this),
    _filterModel(this),
    _treeView(this),
    _selectionModel(&_filterModel)
{
    _filterModel.setSourceModel(&_model);

    _treeView.setModel(&_filterModel);

    _treeView.setContextMenuPolicy(Qt::CustomContextMenu);
    _treeView.setSelectionModel(&_selectionModel);
    _treeView.setDragEnabled(true);
    _treeView.setDragDropMode(QAbstractItemView::DragOnly);
    _treeView.setSelectionBehavior(QAbstractItemView::SelectRows);
    _treeView.setSelectionMode(QAbstractItemView::ExtendedSelection);
    _treeView.setRootIsDecorated(true);
    _treeView.setItemsExpandable(true);
    _treeView.setIconSize(QSize(14, 14));

    _treeView.header()->setStretchLastSection(false);
    _treeView.header()->setMinimumSectionSize(18);

    //_treeView.header()->resizeSection(DataHierarchyModelItem::Column::Name, 180);
    //_treeView.header()->resizeSection(DataHierarchyModelItem::Column::GroupIndex, 60);
    //_treeView.header()->resizeSection(DataHierarchyModelItem::Column::Progress, 45);
    //_treeView.header()->resizeSection(DataHierarchyModelItem::Column::IsGroup, _treeView.header()->minimumSectionSize());
    //_treeView.header()->resizeSection(DataHierarchyModelItem::Column::IsAnalyzing, _treeView.header()->minimumSectionSize());
    //_treeView.header()->resizeSection(DataHierarchyModelItem::Column::IsLocked, _treeView.header()->minimumSectionSize());

    //_treeView.header()->setSectionResizeMode(DataHierarchyModelItem::Column::Name, QHeaderView::Interactive);
    //_treeView.header()->setSectionResizeMode(DataHierarchyModelItem::Column::GUID, QHeaderView::Fixed);
    //_treeView.header()->setSectionResizeMode(DataHierarchyModelItem::Column::GroupIndex, QHeaderView::Fixed);
    //_treeView.header()->setSectionResizeMode(DataHierarchyModelItem::Column::Info, QHeaderView::Stretch);
    //_treeView.header()->setSectionResizeMode(DataHierarchyModelItem::Column::Progress, QHeaderView::Fixed);
    //_treeView.header()->setSectionResizeMode(DataHierarchyModelItem::Column::IsGroup, QHeaderView::Fixed);
    //_treeView.header()->setSectionResizeMode(DataHierarchyModelItem::Column::IsAnalyzing, QHeaderView::Fixed);
    //_treeView.header()->setSectionResizeMode(DataHierarchyModelItem::Column::IsLocked, QHeaderView::Fixed);

    auto layout = new QVBoxLayout();

    layout->addWidget(&_treeView);

    setLayout(layout);
}

}
}
