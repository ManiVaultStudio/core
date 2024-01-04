// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TreeAction.h"

#include "widgets/HierarchyWidget.h"

#include <QVBoxLayout>
#include <QMenu>

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>

namespace mv::gui {

TreeAction::TreeAction(QObject* parent, const QString& title) :
    WidgetAction(parent, title),
    _model(nullptr),
    _filterModel(nullptr)
{
    setDefaultWidgetFlags(WidgetFlag::FilterToolbar);
}

void TreeAction::initialize(QAbstractItemModel* model, QSortFilterProxyModel* filterModel, const QString& itemTypeName)
{
    Q_ASSERT(model != nullptr);

    if (model == nullptr)
        return;

    _model          = model;
    _filterModel    = filterModel;
    _itemTypeName   = itemTypeName;
}

TreeAction::Widget::Widget(QWidget* parent, TreeAction* treeAction, const std::int32_t& widgetFlags) :
    QWidget(parent),
    _treeAction(treeAction)
{
    Q_ASSERT(treeAction != nullptr);

    if (treeAction == nullptr)
        return;

    Q_ASSERT(treeAction->getModel() != nullptr);

    if (treeAction->getModel() == nullptr)
        return;

    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(new HierarchyWidget(this, _treeAction->getItemTypeName(), *_treeAction->getModel(), _treeAction->getFilterModel()));

    setLayout(layout);
}

QWidget* TreeAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    if (dynamic_cast<QMenu*>(parent))
        return QWidgetAction::createWidget(parent);

    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(new TreeAction::Widget(parent, this, widgetFlags));

    widget->setLayout(layout);

    return widget;
}

}
