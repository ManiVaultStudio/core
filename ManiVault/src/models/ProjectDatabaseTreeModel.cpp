// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectDatabaseTreeModel.h"

#include "CoreInterface.h"

#include "util/FileDownloader.h"
#include "util/JSON.h"

#include <nlohmann/json.hpp>

#include <QtConcurrent>

#ifdef _DEBUG
    //#define PROJECT_DATABASE_TREE_MODEL_VERBOSE
#endif

using namespace mv::util;
using namespace mv::gui;

using nlohmann::json;

namespace mv {

ProjectDatabaseTreeModel::ProjectDatabaseTreeModel(QObject* parent /*= nullptr*/) :
    AbstractProjectDatabaseModel(parent),
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

					if (fullJson.contains("Projects")) {
						validateJson(fullJson["Projects"].dump(), _dsnsAction.getStrings()[dsnIndex].toStdString(), loadJsonFromResource(":/JSON/ProjectsSchema"), "https://github.com/ManiVaultStudio/core/tree/master/ManiVault/res/json/ProjectsSchema.json");
					}
					else {
						throw std::runtime_error("/Projects key is missing");
					}

					QJsonParseError jsonParseError;

					const auto jsonDocument = QJsonDocument::fromJson(_future.resultAt<QByteArray>(dsnIndex), &jsonParseError);

					if (jsonParseError.error != QJsonParseError::NoError || !jsonDocument.isObject()) {
						qWarning() << "Invalid JSON from DSN at index" << dsnIndex << ":" << jsonParseError.errorString();
						continue;
					}

					const auto projects = jsonDocument.object()["Projects"].toArray();

					for (const auto project : projects) {
						auto projectMap = project.toVariant().toMap();

						addProject(new ProjectDatabaseProject(projectMap));
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
        connect(&pluginFactory->getProjectsDsnsAction(), &StringsAction::stringsChanged, this, &ProjectDatabaseTreeModel::synchronizeWithDsns);
    }
}

QVariant ProjectDatabaseTreeModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    switch (static_cast<Column>(section))
    {
        case Column::Title:
            return TitleItem::headerData(orientation, role);

        case Column::Tags:
            return TagsItem::headerData(orientation, role);

        case Column::Date:
            return DateItem::headerData(orientation, role);

        case Column::IconName:
            return IconNameItem::headerData(orientation, role);

        case Column::Summary:
            return SummaryItem::headerData(orientation, role);

        case Column::Url:
            return UrlItem::headerData(orientation, role);

        case Column::MinimumCoreVersion:
            return MinimumCoreVersionItem::headerData(orientation, role);

        case Column::RequiredPlugins:
            return RequiredPluginsItem::headerData(orientation, role);

        case Column::MissingPlugins:
            return MissingPluginsItem::headerData(orientation, role);

		case Column::Count:
            break;
    }

    return {};
}

QSet<QString> ProjectDatabaseTreeModel::getTagsSet() const
{
    return _tags;
}

void ProjectDatabaseTreeModel::addProject(const ProjectDatabaseProject* project)
{
    Q_ASSERT(project);

    if (!project)
        return;

    appendRow(Row(project));
    updateTags();

    const_cast<ProjectDatabaseProject*>(project)->setParent(this);

    _projects.push_back(project);
}

void ProjectDatabaseTreeModel::updateTags()
{
    for (int rowIndex = 0; rowIndex < rowCount(); ++rowIndex)
        for (const auto& tag : dynamic_cast<Item*>(itemFromIndex(index(rowIndex, 0)))->getProject()->getTags())
            _tags.insert(tag);

    emit tagsChanged(_tags);
}

const ProjectDatabaseProject* ProjectDatabaseTreeModel::getProject(const QModelIndex& index) const
{
    Q_ASSERT(index.isValid());

    if (!index.isValid())
        return nullptr;

    const auto itemAtIndex = dynamic_cast<Item*>(itemFromIndex(index));

    Q_ASSERT(itemAtIndex);

    if (!itemAtIndex)
        return nullptr;

    return itemAtIndex->getProject();
}

const ProjectDatabaseProjects& ProjectDatabaseTreeModel::getProjects() const
{
	return _projects;
}

void ProjectDatabaseTreeModel::synchronizeWithDsns()
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

QByteArray ProjectDatabaseTreeModel::downloadProjectsFromDsn(const QString& dsn)
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

ProjectDatabaseTreeModel::Item::Item(const mv::util::ProjectDatabaseProject* project, bool editable /*= false*/) :
    _project(project)
{
}

const ProjectDatabaseProject* ProjectDatabaseTreeModel::Item::getProject() const
{
    return _project;
}

QVariant ProjectDatabaseTreeModel::TitleItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getProject()->getTitle();

        case Qt::ToolTipRole:
            return "Title: " + data(Qt::DisplayRole).toString();

		case Qt::DecorationRole:
            return StyledIcon(getProject()->getIconName());

        default:
            break;
    }

    return Item::data(role);
}

QVariant ProjectDatabaseTreeModel::TagsItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return getProject()->getTags();

        case Qt::DisplayRole:
            return data(Qt::EditRole).toStringList().join(",");

        case Qt::ToolTipRole:
            return "Tags: " + data(Qt::DisplayRole).toStringList().join(",");

        default:
            break;
    }

    return Item::data(role);
}

QVariant ProjectDatabaseTreeModel::DateItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getProject()->getDate();

        case Qt::ToolTipRole:
            return "Date: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

QVariant ProjectDatabaseTreeModel::IconNameItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
	    case Qt::EditRole:
	    case Qt::DisplayRole:
	        return getProject()->getIconName();

	    case Qt::ToolTipRole:
	        return "Icon name: " + data(Qt::DisplayRole).toString();

	    default:
	        break;
    }

    return Item::data(role);
}

QVariant ProjectDatabaseTreeModel::SummaryItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getProject()->getSummary();

        case Qt::ToolTipRole:
            return "Summary: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

QVariant ProjectDatabaseTreeModel::UrlItem::data(int role) const
{
    switch (role) {
	    case Qt::EditRole:
            return QVariant::fromValue(getProject()->getUrl());

	    case Qt::DisplayRole:
            return QVariant::fromValue(getProject()->getUrl().toString());

        case Qt::ToolTipRole:
            return "URL: " + data(Qt::DisplayRole).toString();

	    default:
	        break;
    }

    return Item::data(role);
}

QVariant ProjectDatabaseTreeModel::MinimumCoreVersionItem::data(int role) const
{
    switch (role) {
		case Qt::EditRole:
            return QVariant::fromValue(getProject()->getMinimumCoreVersion());

    case Qt::DisplayRole:
	        return QString::fromStdString(data(Qt::EditRole).value<Version>().getVersionString());

	    case Qt::ToolTipRole:
	        return "Minimum ManiVault Studio application core version: " + data(Qt::DisplayRole).toString();

	    default:
	        break;
    }
    
	return Item::data(role);
}

QVariant ProjectDatabaseTreeModel::RequiredPluginsItem::data(int role) const
{
    switch (role) {
	    case Qt::EditRole:
	        return getProject()->getRequiredPlugins();

	    case Qt::DisplayRole:
	        return data(Qt::EditRole).toStringList();

	    case Qt::ToolTipRole:
	        return "Required plugins: " + data(Qt::DisplayRole).toStringList().join(", ");

	    default:
	        break;
    }

    return Item::data(role);
}

QVariant ProjectDatabaseTreeModel::MissingPluginsItem::data(int role) const
{
    switch (role) {
	    case Qt::EditRole:
	        return getProject()->getMissingPlugins();

	    case Qt::DisplayRole:
	        return data(Qt::EditRole).toStringList();

	    case Qt::ToolTipRole:
	        return "Missing plugins: " + data(Qt::DisplayRole).toStringList().join(", ");

	    default:
	        break;
    }

    return Item::data(role);
}

}
