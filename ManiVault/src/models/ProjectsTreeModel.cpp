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

ProjectsTreeModel::ProjectsTreeModel(QObject* parent /*= nullptr*/) :
    AbstractProjectsModel(parent),
    _dsnsAction(this, "Data Source Names")
{
    _dsnsAction.setIconByName("globe");
    _dsnsAction.setToolTip("Projects Data Source Names (DSN)");
    _dsnsAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _dsnsAction.setDefaultWidgetFlags(StringsAction::WidgetFlag::ListView);
    _dsnsAction.setPopupSizeHint(QSize(550, 100));

    connect(&_dsnsAction, &StringsAction::stringsChanged, this, [this]() -> void {
        setRowCount(0);

        if (mv::plugins().isInitializing())
            return;

        _future = QtConcurrent::mapped(
            _dsnsAction.getStrings(),
            [this](const QString& dsn) {
                return downloadProjectsFromDsn(dsn);
	    });

    	connect(&_watcher, &QFutureWatcher<QByteArray>::finished, [&]() {
            for (int dsnIndex = 0; dsnIndex < _dsnsAction.getStrings().size(); ++dsnIndex) {
	            try {
					const auto jsonData = _future.resultAt<QByteArray>(dsnIndex);
					const auto fullJson = json::parse(QString::fromUtf8(jsonData.constData()).toStdString());

					if (fullJson.contains("projects")) {
						validateJson(fullJson["projects"].dump(), _dsnsAction.getStrings()[dsnIndex].toStdString(), loadJsonFromResource(":/JSON/ProjectsSchema"), "https://github.com/ManiVaultStudio/core/tree/master/ManiVault/res/json/ProjectsSchema.json");
					}
					else {
						throw std::runtime_error("/projects key is missing");
					}

					QJsonParseError jsonParseError;

					const auto jsonDocument = QJsonDocument::fromJson(_future.resultAt<QByteArray>(dsnIndex), &jsonParseError);

					if (jsonParseError.error != QJsonParseError::NoError || !jsonDocument.isObject()) {
						qWarning() << "Invalid JSON from DSN at index" << dsnIndex << ":" << jsonParseError.errorString();
						continue;
					}

					const auto projects = jsonDocument.object()["projects"].toArray();

					for (const auto project : projects) {
						auto projectMap = project.toVariant().toMap();

						addProject(new ProjectsModelProject(projectMap), projectMap["group"].toString());
					}

					emit populatedFromDsns();
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
		});

        _watcher.setFuture(_future);
	});

    connect(core(), &CoreInterface::initialized, this, [this]() -> void {
        synchronizeWithDsns();
    });

    for (auto pluginFactory : mv::plugins().getPluginFactoriesByTypes()) {
        connect(&pluginFactory->getProjectsDsnsAction(), &StringsAction::stringsChanged, this, &ProjectsTreeModel::synchronizeWithDsns);
    }
}

void ProjectsTreeModel::synchronizeWithDsns()
{
    if (!mv::core()->isInitialized())
        return;

    QStringList uniqueDsns;

    for (auto pluginFactory : mv::plugins().getPluginFactoriesByTypes()) {
        uniqueDsns << pluginFactory->getProjectsDsnsAction().getStrings();
    }

    uniqueDsns.removeDuplicates();
    
    _dsnsAction.setStrings(uniqueDsns);
}

QByteArray ProjectsTreeModel::downloadProjectsFromDsn(const QString& dsn)
{
    QEventLoop loop;

    QByteArray downloadedData;

    FileDownloader fileDownloader;

    connect(&fileDownloader, &FileDownloader::downloaded, [&]() -> void {
        try
        {
            downloadedData = fileDownloader.downloadedData();
            loop.quit();
        }
        catch (std::exception& e)
        {
            exceptionMessageBox("Unable to download projects JSON from DSN", e);
        }
        catch (...)
        {
            exceptionMessageBox("Unable to download projects JSON from DSN");
        }
    });

    fileDownloader.download(dsn);

    loop.exec();

    return downloadedData;
}

}
