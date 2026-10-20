// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProxyDatasetsAction.h"

#include <QHBoxLayout>

using namespace mv;

ProxyDatasetsAction::ProxyDatasetsAction(QObject* parent, const Dataset<Points>& points) :
    WidgetAction(parent, "Proxy Datsets"),
    _points(points),
    _countAction(this, "Number of proxy datasets"),
    _editProxyDatasetsAction(this, points)
{
    setText("Proxy datasets");
    setToolTip("Proxy datasets");

    _countAction.setEnabled(false);
    _countAction.setToolTip("Number of proxy datasets");

    const auto updateActions = [this]() -> void {
        setEnabled(_points->getProxyMembers().count() >= 1);

        _countAction.setString(QString::number(_points->getProxyMembers().count()));
        _countAction.setToolTip(QString("%1 proxy dataset(s)").arg(QString::number(_points->getProxyMembers().count())));
        _editProxyDatasetsAction.setEnabled(_points->isProxy());
    };

    connect(&_points, &Dataset<Points>::dataChanged, this, updateActions);

    updateActions();
}

ProxyDatasetsAction::Widget::Widget(QWidget* parent, ProxyDatasetsAction* proxyDatasetsAction) :
    WidgetActionWidget(parent, proxyDatasetsAction)
{
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(proxyDatasetsAction->getCountAction().createWidget(this), 1);
    layout->addWidget(proxyDatasetsAction->getEditProxyDatasetsAction().createCollapsedWidget(this));

    setLayout(layout);
}