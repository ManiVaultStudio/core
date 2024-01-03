// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TreeAction.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
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

TreeAction::Widget::Widget(QWidget* parent, TreeAction* treeAction, const std::int32_t& widgetFlags) :
    QWidget(parent),
    _treeAction(treeAction),
    _hierarchyWidget(this, "Item", *treeAction->getModel(), treeAction->getFilterModel())
{
    auto layout = new QVBoxLayout();

    layout->addWidget(&_hierarchyWidget);

    setLayout(layout);
}

QWidget* TreeAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    if (dynamic_cast<QMenu*>(parent))
        return QWidgetAction::createWidget(parent);

    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(new TreeAction::Widget(parent, this, widgetFlags));

    widget->setLayout(layout);

    return widget;
}

}
