#include "PublicActionsWidget.h"

#include <Application.h>

#include <QDebug>

using namespace hdps;

PublicActionsWidget::PublicActionsWidget(QWidget* parent) :
    QWidget(parent),
    _filterModel(this),
    _hierarchyWidget(this, "Public action", const_cast<ActionsModel&>(Application::getActionsManager().getActionsModel()))
{
}

//#include "ActionsViewerWidget.h"
//
//#include <Application.h>
//
//#include <QDebug>
//#include <QVBoxLayout>
//#include <QLabel>
//#include <QResizeEvent>
//#include <QGraphicsOpacityEffect>
//#include <QHeaderView>
//
//#include <stdexcept>
//
//namespace hdps
//{
//
//namespace gui
//{
//
//ActionsViewerWidget::ActionsViewerWidget(QWidget* parent) :
//    QWidget(parent),
//    _treeView(this),
//    _overlayWidget(this),
//    _filterModel(this),
//    _nameFilterAction(this, "Name filter")
//{
//    _filterModel.setSourceModel(const_cast<ActionsModel*>(&Application::getActionsManager().getActionsModel()));
//    _filterModel.setScopeFilter(ActionsFilterModel::ScopeFilter::Public);
//
//    connect(&_filterModel, &QAbstractItemModel::rowsInserted, this, &ActionsViewerWidget::numberOfRowsChanged);
//    connect(&_filterModel, &QAbstractItemModel::rowsRemoved, this, &ActionsViewerWidget::numberOfRowsChanged);
//
//    _treeView.setModel(&_filterModel);
//
//    _treeView.setContextMenuPolicy(Qt::CustomContextMenu);
//    _treeView.setDragEnabled(true);
//    _treeView.setDragDropMode(QAbstractItemView::DragOnly);
//    _treeView.setSelectionMode(QAbstractItemView::SingleSelection);
//    _treeView.setRootIsDecorated(false);
//    _treeView.setItemsExpandable(true);
//    _treeView.setSortingEnabled(true);
//
//    _treeView.header()->setStretchLastSection(false);
//    _treeView.header()->setMinimumSectionSize(18);
//    _treeView.header()->hideSection(ActionsModel::Column::IsPublic);
//
//    _treeView.header()->resizeSection(ActionsModel::Column::Name, 200);
//    _treeView.header()->resizeSection(ActionsModel::Column::Type, 130);
//    _treeView.header()->resizeSection(ActionsModel::Column::IsPublic, 50);
//    _treeView.header()->resizeSection(ActionsModel::Column::NumberOfConnections, 50);
//
//    _treeView.header()->setSectionResizeMode(ActionsModel::Column::Name, QHeaderView::Interactive);
//    _treeView.header()->setSectionResizeMode(ActionsModel::Column::Type, QHeaderView::Interactive);
//    _treeView.header()->setSectionResizeMode(ActionsModel::Column::IsPublic, QHeaderView::Fixed);
//    _treeView.header()->setSectionResizeMode(ActionsModel::Column::NumberOfConnections, QHeaderView::Stretch);
//
//    _nameFilterAction.setPlaceHolderString("Filter by name...");
//    _nameFilterAction.setSearchMode(true);
//
//    connect(&_nameFilterAction, &StringAction::stringChanged, &_filterModel, &ActionsFilterModel::setNameFilter);
//
//    auto layout = new QVBoxLayout();
//
//    layout->setContentsMargins(6, 6, 6, 6);
//    layout->setSpacing(3);
//
//    auto toolbarLayout = new QHBoxLayout();
//
//    toolbarLayout->setContentsMargins(0, 3, 0, 3);
//    toolbarLayout->setSpacing(4);
//    toolbarLayout->addWidget(_nameFilterAction.createWidget(this), 1);
//
//    layout->addLayout(toolbarLayout);
//    layout->addWidget(&_treeView);
//
//    setLayout(layout);
//
//    numberOfRowsChanged();
//}