// SPDX-License-Identifier: LGPL-3.0-or-later

#include "PointsAction.h"

#include "../PointData.h"
#include "../view/PointsViewWidget.h"

#include <CoreInterface.h>
#include <Project.h>
#include <ViewPlugin.h>
#include <util/StyledIcon.h>
#include <event/Event.h>

#include <QSignalBlocker>

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
            effect = QString("Dataset '%1' will use the project's staging policy.").arg(datasetName);
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
    _projectResourceStagingPolicyAction(this, "Resource staging policy", { "Inherit", "Read ranges on demand", "Stage on first read", "Stage immediately" }, "Inherit")
{
    _openInPointsViewAction.setIconByName("arrow-up-right-from-square");
    _openInPointsViewAction.setToolTip("Open this point dataset in the standalone Points View plugin");
    _projectResourceStagingPolicyAction.setToolTip("Choose how this dataset's archive-backed resources are accessed when opening a project. The choice applies from the next project-open session after the project is saved; it does not change the current session. Inherit uses the project's policy.");
    _projectResourceStagingPolicyAction.setDescription(
        "<p>Overrides the project resource staging policy for this point dataset.</p>"
        "<ul><li><b>Inherit:</b> use the project's resource staging policy.</li>"
        "<li><b>Read ranges on demand:</b> read requested data directly from the archive without extracting full resource files.</li>"
        "<li><b>Stage on first read:</b> extract each resource to the temporary directory when first accessed.</li>"
        "<li><b>Stage immediately:</b> extract resources while the project opens, which can increase opening time and temporary disk use.</li></ul>"
        "<p>Save the project and reopen it for this policy to take effect; it does not change the current session.</p>");
    _projectResourceStagingPolicyAction.setDefaultWidgetFlags(mv::gui::OptionAction::ComboBox);

    addAction(&_projectResourceStagingPolicyAction);
    addAction(&_storageBackendAction);
    addAction(&_pointsViewAction);
    addAction(&_openInPointsViewAction);

    const auto updateProjectResourceStagingPolicy = [this] {
        if (!_points.isValid())
            return;

        auto options = _projectResourceStagingPolicyAction.getOptions();
        const auto datasetPolicy = _points->getProjectResourceStagingPolicy();
        const auto effectivePolicy = mv::projects().resolveProjectResourceStagingPolicy(datasetPolicy);

        if (!options.isEmpty())
            options[0] = QStringLiteral("Inherited (%1)").arg(mv::util::projectResourceStagingPolicyName(effectivePolicy));

        const QSignalBlocker blocker(&_projectResourceStagingPolicyAction);
        _projectResourceStagingPolicyAction.setOptions(options);
        _projectResourceStagingPolicyAction.setCurrentText(datasetPolicy == mv::util::ProjectResourceStagingPolicy::Inherit
            ? options.value(0)
            : mv::util::projectResourceStagingPolicyName(datasetPolicy));
    };

    updateProjectResourceStagingPolicy();
    connect(&_projectResourceStagingPolicyAction, &mv::gui::OptionAction::currentTextChanged, this, [this](const QString& value) {
        if (!_points.isValid())
            return;

        const auto policy = value.startsWith(QStringLiteral("Inherited ("))
            ? mv::util::ProjectResourceStagingPolicy::Inherit
            : mv::util::projectResourceStagingPolicyFromName(value);
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

    if (const auto project = mv::projects().getCurrentProject())
        connect(&project->getProjectMetaAction().getProjectResourceStagingPolicyAction(), &mv::gui::OptionAction::currentTextChanged, this, [updateProjectResourceStagingPolicy](const QString&) {
            updateProjectResourceStagingPolicy();
        });

    _eventListener.addSupportedEventType(static_cast<std::uint32_t>(mv::EventType::DatasetDataChanged));
    _eventListener.registerDataEventByType(PointType, [this, updateProjectResourceStagingPolicy](mv::DatasetEvent* dataEvent) {
        if (!_points.isValid() || dataEvent->getDataset()->getId() != _points->getId())
            return;

        updateProjectResourceStagingPolicy();
    });

    connect(&_openInPointsViewAction, &mv::gui::TriggerAction::triggered, this, [this] {
        if (!_pointsViewAction.getPoints().isValid())
            return;

        if (!mv::plugins().getPluginFactory("Points View")) {
            mv::help().addNotification(
                "Points View unavailable",
                "The Points View plugin is not available. Check that it is installed and that its dependencies are available.",
                mv::util::StyledIcon("circle-info"));
            return;
        }

        const auto viewState = _pointsViewAction.getViewState();
        auto* pointsViewPlugin = mv::plugins().requestViewPluginFloated("Points View", { _pointsViewAction.getPoints() });

        if (!pointsViewPlugin) {
            mv::help().addNotification(
                "Could not open Points View",
                "The Points View plugin is available, but a view could not be created.",
                mv::util::StyledIcon("circle-info"));
            return;
        }

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
