// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectDatabaseModel.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>

#ifdef _DEBUG
    //#define PROJECT_DATABASE_MODEL_VERBOSE
#endif

using namespace mv::util;
using namespace mv::gui;

namespace mv {

QMap<ProjectDatabaseModel::Column, ProjectDatabaseModel::ColumHeaderInfo> ProjectDatabaseModel::columnInfo = QMap<Column, ColumHeaderInfo>({
    { Column::Title, { "Title" , "Title", "Title" } },
    { Column::Tags, { "Tags" , "Tags", "Tags" } },
    { Column::Date, { "Date" , "Date", "Issue date" } },
    { Column::IconName, { "Icon Name" , "Icon Name", "Font Awesome icon name" } },
    { Column::Summary, { "Summary" , "Summary", "Summary (brief description)" } },
    { Column::Url, { "URL" , "URL", "Project URL" } },
    { Column::MinimumCoreVersion, { "Min. app core version" , "Min. app core version", "Minimum ManiVault Studio application core version" } },
    { Column::RequiredPlugins, { "Required plugins" , "Required plugins", "Plugins required to open the project" } },
    { Column::MissingPlugins, { "Missing plugins" , "Missing plugins", "List of plugins which are missing" } },
});

ProjectDatabaseModel::ProjectDatabaseModel(QObject* parent /*= nullptr*/) :
    StandardItemModel(parent),
    _dsnsAction(this, "Data Source Names")
{
    setColumnCount(static_cast<int>(Column::Count));

    _dsnsAction.setIconByName("globe");
    _dsnsAction.setToolTip("Project database data source names");
    _dsnsAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _dsnsAction.setEnabled(false);

    connect(&_dsnsAction, &StringsAction::stringsChanged, this, [this]() -> void {
        setRowCount(0);

        QFuture<QByteArray> future = QtConcurrent::mapped(_dsnsAction.getStrings(), &ProjectDatabaseModel::downloadProjectsFromDsn);

        QFutureWatcher<QByteArray> watcher;

    	connect(&watcher, &QFutureWatcher<QString>::finished, [&]() {
            QStringList results;

    		for (int dsnIndex = 0; dsnIndex < _dsnsAction.getStrings().size(); ++dsnIndex) {
                results << future.resultAt(dsnIndex);

                const auto jsonDocument = QJsonDocument::fromJson(future.resultAt(dsnIndex));
                const auto projects     = jsonDocument.object()["Projects"].toArray();

                for (const auto project : projects) {
                    auto projectMap = project.toVariant().toMap();

                    addProject(new ProjectDatabaseProject(projectMap));
                }
    		}

            emit populatedFromDsns();
		});

        watcher.setFuture(future);
	});
}

QVariant ProjectDatabaseModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
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

QSet<QString> ProjectDatabaseModel::getTagsSet() const
{
    return _tags;
}

void ProjectDatabaseModel::addProject(const ProjectDatabaseProject* project)
{
    Q_ASSERT(project);

    if (!project)
        return;

    appendRow(Row(project));
    updateTags();

    const_cast<ProjectDatabaseProject*>(project)->setParent(this);

    _projects.push_back(project);
}

void ProjectDatabaseModel::updateTags()
{
    for (int rowIndex = 0; rowIndex < rowCount(); ++rowIndex)
        for (const auto& tag : dynamic_cast<Item*>(itemFromIndex(index(rowIndex, 0)))->getProject()->getTags())
            _tags.insert(tag);

    emit tagsChanged(_tags);
}

const ProjectDatabaseProject* ProjectDatabaseModel::getProject(const QModelIndex& index) const
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

const ProjectDatabaseProjects& ProjectDatabaseModel::getProjects() const
{
	return _projects;
}

QByteArray ProjectDatabaseModel::downloadProjectsFromDsn(const QString& dsn)
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

ProjectDatabaseModel::Item::Item(const mv::util::ProjectDatabaseProject* project, bool editable /*= false*/) :
    _project(project)
{
}

const ProjectDatabaseProject* ProjectDatabaseModel::Item::getProject() const
{
    return _project;
}

QVariant ProjectDatabaseModel::TitleItem::data(int role /*= Qt::UserRole + 1*/) const
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

QVariant ProjectDatabaseModel::TagsItem::data(int role /*= Qt::UserRole + 1*/) const
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

QVariant ProjectDatabaseModel::DateItem::data(int role /*= Qt::UserRole + 1*/) const
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

QVariant ProjectDatabaseModel::IconNameItem::data(int role /*= Qt::UserRole + 1*/) const
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

QVariant ProjectDatabaseModel::SummaryItem::data(int role /*= Qt::UserRole + 1*/) const
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

QVariant ProjectDatabaseModel::UrlItem::data(int role) const
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

QVariant ProjectDatabaseModel::MinimumCoreVersionItem::data(int role) const
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

QVariant ProjectDatabaseModel::RequiredPluginsItem::data(int role) const
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

QVariant ProjectDatabaseModel::MissingPluginsItem::data(int role) const
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
