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

namespace
{

QStringList storageBackendTypes()
{
    return {
        pointdata::storage::getStorageBackendTypeName(Points::StorageBackendType::Eager),
        pointdata::storage::getStorageBackendTypeName(Points::StorageBackendType::Tiled)
    };
}

}

StorageBackendAction::StorageBackendAction(QObject* parent, const Dataset<Points>& points) :
    StorageBackendConfiguratorAction(parent, storageBackendTypes(), pointdata::storage::getStorageBackendTypeName(Points::StorageBackendType::Eager)),
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
    const auto backendType    = pointsAreValid ? _points->getStorageBackendType() : std::optional<Points::StorageBackendType>();

    getBackendTypeAction().setCurrentText(backendType ? pointdata::storage::getStorageBackendTypeName(*backendType) : QString());

    getBackendTypeAction().setEnabled(pointsAreValid && !isProxy && !_conversionInProgress);
    setConfigureEnabled(pointsAreValid && !isProxy && backendType && *backendType == Points::StorageBackendType::Tiled && !_conversionInProgress);

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

    if (!currentBackendType)
    {
        updateBackendType();
        return;
    }

    if (backendType == "Configure")
    {
        const auto targetType = *currentBackendType;
        auto configurationAction = _points->createStorageBackendConversionAction(targetType);

        if (!configurationAction)
        {
            updateBackendType();
            return;
        }

        StorageBackendConfigurationDialog dialog(std::move(configurationAction), StorageBackendConfigurationDialog::Purpose::Configure, QApplication::activeWindow());

        if (dialog.exec() == QDialog::Accepted)
            convertBackend(targetType, dialog.getConversionOptions());
        else
            updateBackendType();

        return;
    }

    const auto requestedBackendType = pointdata::storage::getStorageBackendType(backendType);

    if (!requestedBackendType || *requestedBackendType == *currentBackendType)
    {
        updateBackendType();
        return;
    }

    if (*requestedBackendType == Points::StorageBackendType::Tiled)
    {
        auto configurationAction = _points->createStorageBackendConversionAction(Points::StorageBackendType::Tiled);
        StorageBackendConfigurationDialog dialog(std::move(configurationAction), StorageBackendConfigurationDialog::Purpose::Convert, QApplication::activeWindow());

        if (dialog.exec() != QDialog::Accepted)
        {
            updateBackendType();
            return;
        }

        convertBackend(Points::StorageBackendType::Tiled, dialog.getConversionOptions());
        return;
    }

    if (*requestedBackendType == Points::StorageBackendType::Eager)
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
