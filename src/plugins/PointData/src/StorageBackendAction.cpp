// SPDX-License-Identifier: LGPL-3.0-or-later

#include "StorageBackendAction.h"

#include "StorageBackendConfigurationDialog.h"

#include "PointData.h"

#include <ForegroundTask.h>
#include <util/StyledIcon.h>

#include <QApplication>
#include <QCoreApplication>

#include <exception>

using namespace mv;
using namespace mv::gui;

StorageBackendAction::StorageBackendAction(QObject* parent, const Dataset<Points>& points) :
    HorizontalGroupAction(parent, "Storage backend"),
    _points(points),
    _backendTypeAction(this, "Type", { "Eager", "Tiled", "Mixed", "Unknown" }, "Eager"),
    _configureAction(this, "Configure...")
{
    setShowLabels(true);

    _backendTypeAction.setDefaultWidgetFlags(OptionAction::ComboBox);
    _backendTypeAction.setToolTip("Select the storage backend for this points dataset");

    _configureAction.setIconByName("ellipsis");
    _configureAction.setToolTip("Configure tiled storage and convert this points dataset");

    addAction(&_backendTypeAction);
    addAction(&_configureAction, TriggerAction::Icon);

    connect(&_backendTypeAction, &OptionAction::currentTextChanged, this, [this](const QString& backendType) {
        changeBackendType(backendType);
    });

    connect(&_configureAction, &TriggerAction::triggered, this, [this] {
        changeBackendType("Tiled");
    });

    if (_points.isValid())
        connect(&_points, &Dataset<Points>::dataChanged, this, &StorageBackendAction::updateBackendType);

    updateBackendType();
}

OptionAction& StorageBackendAction::getBackendTypeAction()
{
    return _backendTypeAction;
}

TriggerAction& StorageBackendAction::getConfigureAction()
{
    return _configureAction;
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
            _backendTypeAction.setCurrentText("Eager");
            break;

        case Points::StorageBackendType::Tiled:
            _backendTypeAction.setCurrentText("Tiled");
            break;

        case Points::StorageBackendType::Mixed:
            _backendTypeAction.setCurrentText("Mixed");
            break;

        case Points::StorageBackendType::Unknown:
            _backendTypeAction.setCurrentText("Unknown");
            break;
    }

    _backendTypeAction.setEnabled(pointsAreValid && !isProxy && !_conversionInProgress);
    _configureAction.setEnabled(pointsAreValid && !isProxy && backendType == Points::StorageBackendType::Eager && !_conversionInProgress);

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

    if (backendType == "Tiled")
    {
        auto configurationAction = _points->createStorageBackendConversionAction(Points::StorageBackendType::Tiled);
        StorageBackendConfigurationDialog dialog(std::move(configurationAction), QApplication::activeWindow());

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
