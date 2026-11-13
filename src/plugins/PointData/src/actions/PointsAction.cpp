// SPDX-License-Identifier: LGPL-3.0-or-later

#include "PointsAction.h"

#include "../PointData.h"
#include "../view/PointsViewWidget.h"

#include <CoreInterface.h>
#include <ViewPlugin.h>
#include <util/StyledIcon.h>

namespace
{

QString projectResourceStagingPolicyDescription(const QString& datasetName, const mv::util::ProjectResourceStagingPolicy policy)
{
    QString effect;

    switch (policy)
    {
        case mv::util::ProjectResourceStagingPolicy::ReadRangesOnDemand:
            effect = QString("Dataset '%1' will read requested ranges directly from the project archive as needed, without extracting full resource files to local staging.").arg(datasetName);
            break;

        case mv::util::ProjectResourceStagingPolicy::StageOnFirstRead:
            effect = QString("Dataset '%1' will extract each archive-backed resource to a local staging file the first time it is accessed; later reads use that file.").arg(datasetName);
            break;

        case mv::util::ProjectResourceStagingPolicy::StageImmediately:
            effect = QString("Dataset '%1' will extract all of its archive-backed resources to local staging files while the project opens, before they are first accessed. This can increase project-open time and local disk usage.").arg(datasetName);
            break;

        case mv::util::ProjectResourceStagingPolicy::Inherit:
            effect = QString("Dataset '%1' will inherit the project's staging policy, or the application default if the project also inherits.").arg(datasetName);
            break;
    }

    return QString("%1 This takes effect the next time the saved project is opened; the current session and other already-open sessions are unchanged. Save the project to keep this choice.").arg(effect);
}

}

PointsAction::PointsAction(QObject* parent, const mv::Dataset<Points>& points) :
    GroupAction(parent, "Points", true),
    _points(points),
    _storageBackendAction(this, points),
    _pointsViewAction(this, points),
    _openInPointsViewAction(this, "Open in Points View"),
    _projectResourceStagingPolicyAction(this, "Project resource staging", { "Inherit", "Read ranges on demand", "Stage on first read", "Stage immediately" }, "Inherit")
{
    _openInPointsViewAction.setIconByName("external-link");
    _openInPointsViewAction.setToolTip("Open this point dataset in the standalone Points View plugin");
    _projectResourceStagingPolicyAction.setToolTip("Choose how this dataset's archive-backed resources are accessed when opening a project. The choice applies from the next project-open session after the project is saved; it does not change the current session. Inherit uses the project or application default.");
    _projectResourceStagingPolicyAction.setDefaultWidgetFlags(mv::gui::OptionAction::ComboBox);

    addAction(&_projectResourceStagingPolicyAction);
    addAction(&_storageBackendAction);
    addAction(&_pointsViewAction);
    addAction(&_openInPointsViewAction);

    const auto updateProjectResourceStagingPolicy = [this] {
        if (!_points.isValid())
            return;

        const auto isProxy = _points->isProxy();
        _projectResourceStagingPolicyAction.setEnabled(!isProxy);
        _projectResourceStagingPolicyAction.setCurrentText(mv::util::projectResourceStagingPolicyName(_points->getProjectResourceStagingPolicy()));
    };

    updateProjectResourceStagingPolicy();
    connect(&_projectResourceStagingPolicyAction, &mv::gui::OptionAction::currentTextChanged, this, [this](const QString& value) {
        if (!_points.isValid() || _points->isProxy())
            return;

        const auto policy = mv::util::projectResourceStagingPolicyFromName(value);
        if (_points->getProjectResourceStagingPolicy() == policy)
            return;

        _points->setProjectResourceStagingPolicy(policy);
        mv::events().notifyDatasetDataChanged(_points);
        mv::help().addNotification(
            "Dataset resource staging policy changed",
            projectResourceStagingPolicyDescription(_points->getGuiName(), policy),
            mv::util::StyledIcon("hard-drive"));
    });
    connect(&points, &mv::Dataset<Points>::dataChanged, this, updateProjectResourceStagingPolicy);

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

mv::gui::OptionAction& PointsAction::getProjectResourceStagingPolicyAction()
{
    return _projectResourceStagingPolicyAction;
}
