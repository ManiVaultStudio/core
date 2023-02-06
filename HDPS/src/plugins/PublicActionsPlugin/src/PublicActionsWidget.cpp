#include "PublicActionsWidget.h"

#include <CoreInterface.h>
#include <AbstractActionsManager.h>
#include <Application.h>

#include <models/ActionsModel.h>

#include <QDebug>
#include <QHeaderView>

using namespace hdps;

PublicActionsWidget::PublicActionsWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _model(this),
    _filterModel(this),
    _hierarchyWidget(this, "Shared Parameter", _model, &_filterModel)
{
    _hierarchyWidget.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("cloud"));
    _hierarchyWidget.getTreeView().setRootIsDecorated(true);

    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(&_hierarchyWidget);

    setLayout(layout);

    auto& treeView = _hierarchyWidget.getTreeView();

    treeView.setColumnHidden(static_cast<int>(ActionsModel::Column::ID), true);
    treeView.setColumnHidden(static_cast<int>(ActionsModel::Column::Scope), true);

    auto treeViewHeader = treeView.header();

    treeViewHeader->setStretchLastSection(false);

    treeViewHeader->resizeSection(static_cast<int>(ActionsModel::Column::Name), 300);
    treeViewHeader->resizeSection(static_cast<int>(ActionsModel::Column::ID), 100);
    treeViewHeader->resizeSection(static_cast<int>(ActionsModel::Column::Type), 60);
    treeViewHeader->resizeSection(static_cast<int>(ActionsModel::Column::Scope), 60);

    treeViewHeader->setSectionResizeMode(0, QHeaderView::Stretch);
    treeViewHeader->setSectionResizeMode(1, QHeaderView::Fixed);
}
