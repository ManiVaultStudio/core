// SPDX-License-Identifier: LGPL-3.0-or-later

#include "StorageBackendAction.h"

#include "../storage/interface/StorageBackendConfigurationDialog.h"

#include "../PointData.h"

#include <ModalTask.h>
#include <Application.h>
#include <util/StyledIcon.h>

#include <QApplication>
#include <QCoreApplication>
#include <QElapsedTimer>

#include <exception>
#include <stdexcept>

using namespace mv;
using namespace mv::gui;

namespace
{

struct StorageBackendConversionCancelled
{
};

QStringList storageBackendTypes()
{
    return {
        pointdata::storage::getStorageBackendTypeName(Points::StorageBackendType::Dense),
        pointdata::storage::getStorageBackendTypeName(Points::StorageBackendType::Tiled),
        pointdata::storage::getStorageBackendTypeName(Points::StorageBackendType::Sparse)
    };
}

}

StorageBackendAction::StorageBackendAction(QObject* parent, const Dataset<Points>& points) :
    StorageBackendConfiguratorAction(parent, storageBackendTypes(), pointdata::storage::getStorageBackendTypeName(Points::StorageBackendType::Dense)),
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

    setBackendType(backendType ? pointdata::storage::getStorageBackendTypeName(*backendType) : QString());

    getBackendTypeAction().setEnabled(pointsAreValid && !isProxy && !_conversionInProgress);
    setConfigureEnabled(pointsAreValid && !isProxy && backendType && !_conversionInProgress);

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

    auto conversionAction = _points->createStorageBackendConversionAction(*requestedBackendType);
    if (!conversionAction)
    {
        help().addNotification("Storage conversion unavailable", QString("Converting from %1 to %2 is not supported for this dataset.")
            .arg(pointdata::storage::getStorageBackendTypeName(*currentBackendType), pointdata::storage::getStorageBackendTypeName(*requestedBackendType)), util::StyledIcon("circle-exclamation"));
        updateBackendType();
        return;
    }

    StorageBackendConfigurationDialog dialog(std::move(conversionAction), StorageBackendConfigurationDialog::Purpose::Convert, QApplication::activeWindow());
    if (dialog.exec() == QDialog::Accepted)
        convertBackend(*requestedBackendType, dialog.getConversionOptions());
    else
        updateBackendType();
}

void StorageBackendAction::convertBackend(const pointdata::storage::StorageBackendType type, const pointdata::storage::StorageBackendConfigurationOptionsPtr options)
{
    _conversionInProgress = true;
    updateBackendType();

    const auto sourceType = _points->getStorageBackendType();
    const auto rows = _points->getNumPoints();
    const auto columns = _points->getNumDimensions();
    const auto sourceBytes = _points->getRawDataSize();
    QElapsedTimer conversionTimer;
    conversionTimer.start();
    qInfo() << "Point-data storage conversion started:"
        << (sourceType ? pointdata::storage::getStorageBackendTypeName(*sourceType) : QStringLiteral("unknown"))
        << "->" << pointdata::storage::getStorageBackendTypeName(type)
        << "dimensions" << rows << "x" << columns
        << "source storage bytes" << sourceBytes;

    ModalTask conversionTask(Application::current(), "Convert point-data storage", Task::Status::Running, true);
    conversionTask.setAlwaysProcessEvents(true);
    conversionTask.setProgressDescription("Converting point-data storage");
    conversionTask.setProgress(0.0f);
    QCoreApplication::processEvents();
    std::size_t progressCallbackCount = 0;

    try
    {
        if (conversionTask.isAboutToBeAborted() || conversionTask.isAborting() || conversionTask.isAborted())
            throw StorageBackendConversionCancelled {};

        _points->convertToStorageBackend(type, options, [&conversionTask, &progressCallbackCount](const float progress) {
            ++progressCallbackCount;
            conversionTask.setProgress(progress);

            // Keep the task's cancel control responsive while conversion is in a long-running row or tile loop.
            // A final progress update is emitted after the new backend may already have been installed.
            if (progress < 1.0f)
            {
                QCoreApplication::processEvents();

                if (conversionTask.isAboutToBeAborted() || conversionTask.isAborting() || conversionTask.isAborted())
                    throw StorageBackendConversionCancelled {};
            }
        });

        const auto resultingType = _points->getStorageBackendType();
        if (!resultingType || *resultingType != type)
            throw std::runtime_error("Storage conversion completed without installing the requested backend");

        qInfo() << "Point-data storage conversion completed in" << conversionTimer.elapsed() << "ms; backend:"
            << pointdata::storage::getStorageBackendTypeName(*resultingType)
            << "result storage bytes" << _points->getRawDataSize()
            << "progress callbacks" << progressCallbackCount;

        conversionTask.setProgress(1.0f);
        conversionTask.setFinished();
        help().addNotification("Point-data storage converted", "The point-data storage backend was converted successfully.", util::StyledIcon("check"));
    }
    catch (const StorageBackendConversionCancelled&)
    {
        qInfo() << "Point-data storage conversion cancelled after" << conversionTimer.elapsed() << "ms; progress callbacks" << progressCallbackCount;
        conversionTask.setAborted();
        help().addNotification("Point-data storage conversion cancelled", "The storage conversion was cancelled.", util::StyledIcon("circle-exclamation"));
    }
    catch (const std::exception& exception)
    {
        qWarning() << "Point-data storage conversion failed after" << conversionTimer.elapsed() << "ms; progress callbacks" << progressCallbackCount << ":" << exception.what();
        conversionTask.setAborted();
        help().addNotification("Point-data storage conversion failed", QString::fromUtf8(exception.what()), util::StyledIcon("circle-exclamation"));
    }
    catch (...)
    {
        qWarning() << "Point-data storage conversion failed after" << conversionTimer.elapsed() << "ms; progress callbacks" << progressCallbackCount << "with an unknown exception";
        conversionTask.setAborted();
        help().addNotification("Point-data storage conversion failed", "An unknown error occurred during storage conversion.", util::StyledIcon("circle-exclamation"));
    }

    _conversionInProgress = false;
    updateBackendType();
}
