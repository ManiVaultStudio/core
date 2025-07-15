// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectsTreeModel.h"

#include "CoreInterface.h"

#include "util/FileDownloader.h"
#include "util/JSON.h"

#include <nlohmann/json.hpp>

#include <QtConcurrent>

#ifdef _DEBUG
    //#define PROJECTS_TREE_MODEL_VERBOSE
#endif

using namespace mv::util;
using namespace mv::gui;

using nlohmann::json;

namespace mv {

ProjectsTreeModel::ProjectsTreeModel(const PopulationMode& populationMode /*= Mode::Automatic*/, QObject* parent /*= nullptr*/) :
    AbstractProjectsModel(populationMode, parent),
    _dsnsAction(this, "Data Source Names")
{
    _dsnsAction.setIconByName("globe");
    _dsnsAction.setToolTip("Projects Data Source Names (DSN)");
    _dsnsAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _dsnsAction.setDefaultWidgetFlags(StringsAction::WidgetFlag::ListView);
    _dsnsAction.setPopupSizeHint(QSize(550, 100));

    if (getPopulationMode() == PopulationMode::Automatic || getPopulationMode() == PopulationMode::AutomaticSynchronous) {
        connect(&_dsnsAction, &StringsAction::stringsChanged, this, &ProjectsTreeModel::populateFromDsns);

        connect(core(), &CoreInterface::initialized, this, [this]() -> void {
            populateFromPluginDsns();
        });

        for (auto pluginFactory : mv::plugins().getPluginFactoriesByTypes())
            connect(&pluginFactory->getProjectsDsnsAction(), &StringsAction::stringsChanged, this, &ProjectsTreeModel::populateFromPluginDsns);
    }
}

void ProjectsTreeModel::populateFromDsns()
{
    if (mv::plugins().isInitializing())
        return;

    switch (getPopulationMode()) {
		case PopulationMode::Automatic:
		{
            for (const auto& dsn : _dsnsAction.getStrings()) {
                const auto dsnIndex = _dsnsAction.getStrings().indexOf(dsn);

                FileDownloader::downloadToByteArrayAsync(dsn)
                    .then(this, [this, dsn, dsnIndex](const QByteArray& data) {
	                    try {
                            populateFromJsonByteArray(data, dsnIndex, dsn);
	                    }
	                    catch (std::exception& e)
	                    {
	                        qCritical() << "Unable to add projects from DSN:" << e.what();
	                    }
	                    catch (...)
	                    {
	                        qCritical() << "Unable to add projects from DSN due to an unhandled exception";
	                    }
					})
                    .onFailed(this, [this, dsn, dsnIndex](const QException& e) {
						qWarning().noquote() << "Download failed for" << dsn << ":" << e.what();
					});
            }

            break;
		}

        case PopulationMode::AutomaticSynchronous:
        {
            for (const auto& dsn : _dsnsAction.getStrings()) {
                try {
                    const auto data = FileDownloader::downloadToByteArraySync(dsn);

                    populateFromJsonByteArray(data, _dsnsAction.getStrings().indexOf(dsn), dsn);
                }
                catch (std::exception& e)
                {
                    qCritical() << "Unable to add projects from DSN:" << e.what();
                }
                catch (...)
                {
                    qCritical() << "Unable to add projects from DSN due to an unhandled exception";
                }
            }

            break;
        }

        case PopulationMode::Manual:
            break;
    }
}

void ProjectsTreeModel::populateFromPluginDsns()
{
    try {
        if (!mv::core()->isInitialized())
            return;

        QStringList uniqueDsns;

        for (auto pluginFactory : mv::plugins().getPluginFactoriesByTypes())
            uniqueDsns << pluginFactory->getProjectsDsnsAction().getStrings();

        uniqueDsns.removeDuplicates();

        _dsnsAction.setStrings(uniqueDsns);
	}
	catch (std::exception& e)
	{
	    qCritical() << "Unable to populate projects tree model from plugins DSNs:" << e.what();
	}
	catch (...)
	{
	    qCritical() << "Unable to populate projects tree model from plugins DSNs due to an unhandled exception";
	}
}

void ProjectsTreeModel::populateFromJsonByteArray(const QByteArray& jsonByteArray, std::int32_t dsnIndex, const QString& jsonLocation)
{
    try {
        if (jsonByteArray.isEmpty())
            throw std::runtime_error("JSON byte array is empty");

        const auto fullJson = json::parse(QString::fromUtf8(jsonByteArray.constData()).toStdString());

        if (fullJson.contains("projects")) {
            validateJson(fullJson["projects"].dump(), jsonLocation.toStdString(), loadJsonFromResource(":/JSON/ProjectsSchema"), "https://github.com/ManiVaultStudio/core/tree/master/ManiVault/res/json/ProjectsSchema.json");
        }
        else {
            throw std::runtime_error("/projects key is missing");
        }

        QJsonParseError jsonParseError;

        const auto jsonDocument = QJsonDocument::fromJson(jsonByteArray, &jsonParseError);

        if (jsonParseError.error != QJsonParseError::NoError || !jsonDocument.isObject())
            throw std::runtime_error(QString("Invalid JSON from DSN at index %1: %2").arg(QString::number(dsnIndex), jsonParseError.errorString()).toStdString());

        const auto projects = jsonDocument.object()["projects"].toArray();

        for (const auto project : projects) {
            auto projectMap = project.toVariant().toMap();

            addProject(std::make_shared<ProjectsModelProject>(projectMap), projectMap["group"].toString());
        }
    }
    catch (std::exception& e)
    {
        qCritical() << "Unable to populate projects tree model from JSON content:" << e.what();
    }
    catch (...)
    {
        qCritical() << "Unable to populate projects tree model from JSON content due to an unhandled exception";
    }
}

void ProjectsTreeModel::populateFromJsonFile(const QString& filePath)
{
    try {
        if (filePath.isEmpty())
            throw std::runtime_error("JSON file path is empty");

        QFile file(filePath);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            throw std::runtime_error(QString("Failed to open file: %1").arg(file.errorString()).toStdString());

        populateFromJsonByteArray(file.readAll(), -1, filePath);
    }
    catch (std::exception& e)
    {
        qCritical() << "Unable to populate projects tree model from JSON file:" << e.what();
    }
    catch (...)
    {
        qCritical() << "Unable to populate projects tree model from JSON file due to an unhandled exception";
    }
}

}
