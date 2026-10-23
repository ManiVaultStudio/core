// SPDX-License-Identifier: LGPL-3.0-or-later

#include "StorageBackendAction.h"

#include "../storage/widgets/StorageBackendConfigurationDialog.h"

#include "../PointData.h"

#include <ForegroundTask.h>
#include <util/StyledIcon.h>

#include <QApplication>
#include <QCoreApplication>

#include <exception>

using namespace mv;
using namespace mv::gui;

StorageBackendAction::StorageBackendAction(QObject* parent, const Dataset<Points>& points) :
    StorageBackendConfiguratorAction(parent, { "Eager", "Tiled", "Unknown" }, "Eager"),
    _points(points)
{
    connect(this, &StorageBackendConfiguratorAction::backendTypeChanged, this, [this](const QString& backendType) {
        changeBackendType(backendType);
    });

    connect(this, &StorageBackendConfiguratorAction::configureRequested, this, [this] {
        changeBackendType("Configure");
    });

    if (_points.isValid())
        connect(&_points, &Dataset<Points>::dataChanged, this, &StorageBackendAction::updateBackendType);

    updateBackendType();
}

void StorageBackendAction::updateBackendType()
{
    _updatingBackendType = true;

    const auto pointsAreValid = _points.isValid();
    const auto isProxy        = pointsAreValid && _points->isProxy();
    const auto backendType    = pointsAreValid ? _points->getStorageBackendType() : Points::StorageBackendType::Unknown;

    switch (backendType)
    {
        case Points::StorageBackendType::Eager:
            getBackendTypeAction().setCurrentText("Eager");
            break;

        case Points::StorageBackendType::Tiled:
            getBackendTypeAction().setCurrentText("Tiled");
            break;

        case Points::StorageBackendType::Mixed:
            getBackendTypeAction().setCurrentText("Mixed");
            break;

        case Points::StorageBackendType::Unknown:
            getBackendTypeAction().setCurrentText("Unknown");
            break;
    }

    getBackendTypeAction().setEnabled(pointsAreValid && !isProxy && !_conversionInProgress);
    setConfigureEnabled(pointsAreValid && !isProxy && backendType == Points::StorageBackendType::Tiled && !_conversionInProgress);

    _updatingBackendType = false;
}

void StorageBackendAction::changeBackendType(const QString& backendType)
{
    if (_updatingBackendType)
        return;

    if (_conversionInProgress || !_points.isValid() || _points->isProxy())
    {
        updateBackendType();
        return;
    }

    const auto currentBackendType = _points->getStorageBackendType();

    if ((backendType == "Eager" && currentBackendType == Points::StorageBackendType::Eager) ||
        (backendType == "Tiled" && currentBackendType == Points::StorageBackendType::Tiled))
        return;

    if (backendType == "Configure")
    {
        const auto targetType = currentBackendType;
        auto configurationAction = _points->createStorageBackendConversionAction(targetType);

        if (!configurationAction)
        {
            updateBackendType();
            return;
        }

        StorageBackendConfigurationDialog dialog(std::move(configurationAction), true, QApplication::activeWindow());

        if (dialog.exec() == QDialog::Accepted)
            convertBackend(targetType, dialog.getConversionOptions());
        else
            updateBackendType();

        return;
    }

    if (backendType == "Tiled")
    {
        auto configurationAction = _points->createStorageBackendConversionAction(Points::StorageBackendType::Tiled);
        StorageBackendConfigurationDialog dialog(std::move(configurationAction), false, QApplication::activeWindow());

        if (dialog.exec() != QDialog::Accepted)
        {
            updateBackendType();
            return;
        }

        convertBackend(Points::StorageBackendType::Tiled, dialog.getConversionOptions());
        return;
    }

    if (backendType == "Eager")
    {
        convertBackend(Points::StorageBackendType::Eager);
        return;
    }

    updateBackendType();
}

void StorageBackendAction::convertBackend(const pointdata::storage::StorageBackendType type, const pointdata::storage::StorageBackendConversionOptionsPtr options)
{
    _conversionInProgress = true;
    updateBackendType();

    ForegroundTask conversionTask(this, "Convert point-data storage", Task::Status::Running);
    conversionTask.setAlwaysProcessEvents(true);
    conversionTask.setProgressDescription("Converting point-data storage");

    try
    {
        _points->convertToStorageBackend(type, options, [&conversionTask](const float progress) {
            conversionTask.setProgress(progress);
            QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        });

        conversionTask.setProgress(1.0f);
        conversionTask.setFinished();
        help().addNotification("Point-data storage converted", "The point-data storage backend was converted successfully.", util::StyledIcon("check"));
    }
    catch (const std::exception& exception)
    {
        conversionTask.setAborted();
        help().addNotification("Point-data storage conversion failed", QString::fromUtf8(exception.what()), util::StyledIcon("circle-exclamation"));
    }

    _conversionInProgress = false;
    updateBackendType();
}
