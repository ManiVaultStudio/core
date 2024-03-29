// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "EditProxyDatasetsAction.h"

#include <Application.h>
#include <DataHierarchyItem.h>

#include <QHBoxLayout>
#include <QListView>
#include <QStringListModel>

EditProxyDatasetsAction::EditProxyDatasetsAction(QObject* parent, const Dataset<Points>& points) :
    WidgetAction(parent, "Edit Proxy Datasets"),
    _points(points)
{
    setToolTip("View proxy datasets");
    setIconByName("list");
}

mv::Dataset<Points>& EditProxyDatasetsAction::getPoints()
{
    return _points;
}

EditProxyDatasetsAction::Widget::Widget(QWidget* parent, EditProxyDatasetsAction* editProxyDatasetsAction) :
    WidgetActionWidget(parent, editProxyDatasetsAction)
{
    auto layout     = new QHBoxLayout();
    auto listView   = new QListView();

    listView->setFixedHeight(100);

    layout->addWidget(listView);

    setLayout(layout);
    
    const auto updateListView = [this, editProxyDatasetsAction, listView]() -> void {
        QStringList proxyMemberNames;

        for (const auto& proxyMember : editProxyDatasetsAction->getPoints()->getProxyMembers())
            proxyMemberNames << proxyMember->getDataHierarchyItem().getLocation();

        listView->setModel(new QStringListModel(proxyMemberNames));
    };

    connect(&editProxyDatasetsAction->getPoints(), &Dataset<Points>::dataChanged, this, updateListView);

    updateListView();
}
