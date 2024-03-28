// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TableAction.h"

#include <QVBoxLayout>

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>

namespace mv::gui {

TableAction::TableAction(QObject* parent, const QString& title) :
    VerticalGroupAction(parent, title),
    _model(nullptr),
    _filterModel(nullptr)
{
}

void TableAction::initialize(QAbstractItemModel* model, QSortFilterProxyModel* filterModel, const QString& itemTypeName)
{
    Q_ASSERT(model != nullptr);

    if (model == nullptr)
        return;

    _model          = model;
    _filterModel    = filterModel;
    _itemTypeName   = itemTypeName;

    if (_filterModel)
        _filterModel->setSourceModel(_model);
}

TableAction::Widget::Widget(QWidget* parent, TableAction* tableAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, tableAction, widgetFlags),
    _tableAction(tableAction),
    _toolbarGroupAction(this, "Toolbar"),
    _modelFilterAction(this, "Filter"),
    _tableView()
{
    _toolbarGroupAction.setShowLabels(false);
    _toolbarGroupAction.addAction(&_modelFilterAction);

    _tableView.setObjectName("TableView");

    if (auto filterModel = tableAction->getFilterModel())
        _tableView.setModel(filterModel);
    else
        _tableView.setModel(tableAction->getModel());

    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(_toolbarGroupAction.createWidget(this));
    layout->addWidget(&_tableView);

    setLayout(layout);

    _modelFilterAction.initialize(tableAction->getModel(), tableAction->getFilterModel(), tableAction->getItemTypeName());

    //connect(&_model, &QAbstractItemModel::layoutChanged, this, &HierarchyWidget::updateFilterModel);
}

}
