// SPDX-License-Identifier: LGPL-3.0-or-later

#include "PointsAction.h"

#include "../PointData.h"
#include "../view/PointsViewWidget.h"

#include <CoreInterface.h>
#include <ViewPlugin.h>

PointsAction::PointsAction(QObject* parent, const mv::Dataset<Points>& points) :
    GroupAction(parent, "Points", true),
    _storageBackendAction(this, points),
    _pointsViewAction(this, points),
    _openInPointsViewAction(this, "Open in Points View")
{
    _openInPointsViewAction.setIconByName("external-link");
    _openInPointsViewAction.setToolTip("Open this point dataset in the standalone Points View plugin");

    addAction(&_storageBackendAction);
    addAction(&_pointsViewAction);
    addAction(&_openInPointsViewAction);

    connect(&_openInPointsViewAction, &mv::gui::TriggerAction::triggered, this, [this] {
        if (!_pointsViewAction.getPoints().isValid())
            return;

        const auto viewState = _pointsViewAction.getViewState();
        auto* pointsViewPlugin = mv::plugins().requestViewPluginFloated("Points View", { _pointsViewAction.getPoints() });

        if (!pointsViewPlugin)
            return;

        if (auto* pointsViewWidget = pointsViewPlugin->getWidget().findChild<PointsViewWidget*>())
            pointsViewWidget->setViewState(viewState);
    });
}

StorageBackendAction& PointsAction::getStorageBackendAction()
{
    return _storageBackendAction;
}

PointsViewAction& PointsAction::getPointsViewAction()
{
    return _pointsViewAction;
}

mv::gui::TriggerAction& PointsAction::getOpenInPointsViewAction()
{
    return _openInPointsViewAction;
}
