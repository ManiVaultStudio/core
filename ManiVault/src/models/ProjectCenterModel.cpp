// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectCenterModel.h"

#include <QJsonObject>
#include <QJsonArray>

#ifdef _DEBUG
    //#define PROJECT_CENTER_MODEL_VERBOSE
#endif

using namespace mv::util;
using namespace mv::gui;

namespace mv {

QMap<ProjectCenterModel::Column, ProjectCenterModel::ColumHeaderInfo> ProjectCenterModel::columnInfo = QMap<Column, ColumHeaderInfo>({
    { Column::Title, { "Title" , "Title", "Title" } },
    { Column::Tags, { "Tags" , "Tags", "Tags" } },
    { Column::Date, { "Date" , "Date", "Issue date" } },
    { Column::IconName, { "Icon Name" , "Icon Name", "Font Awesome icon name" } },
    { Column::Summary, { "Summary" , "Summary", "Summary (brief description)" } },
    { Column::Url, { "URL" , "URL", "Project URL" } },
    { Column::MinimumVersionMajor, { "Min. app version (major)" , "Min. app version (major)", "Minimum ManiVault Studio application version (major)" } },
    { Column::MinimumVersionMinor, { "Min. app version (minor)" , "Min. app version (minor)", "Minimum ManiVault Studio application version (minor)" } }
});

ProjectCenterModel::ProjectCenterModel(QObject* parent /*= nullptr*/) :
    StandardItemModel(parent)
{
    setColumnCount(static_cast<int>(Column::Count));

    connect(&_fileDownloader, &FileDownloader::downloaded, this, [this]() -> void {
        
        try
        {
            const auto jsonDocument = QJsonDocument::fromJson(_fileDownloader.downloadedData());
            const auto projects     = jsonDocument.object()["cytosploreViewerProjectDownloads"].toArray();

            for (const auto project : projects) {
                auto projectMap = project.toVariant().toMap();

                qDebug() << "Project center project:" << projectMap["projectName"];
                addProject(new ProjectCenterProject(projectMap));
            }

            emit populatedFromSourceUrl();
        }
        catch (std::exception& e)
        {
            exceptionMessageBox("Unable to process project center JSON", e);
        }
        catch (...)
        {
            exceptionMessageBox("Unable to process project center JSON");
        }
	});
}

QVariant ProjectCenterModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
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

        case Column::MinimumVersionMajor:
            return MinimumVersionMajorItem::headerData(orientation, role);

        case Column::MinimumVersionMinor:
            return MinimumVersionMinorItem::headerData(orientation, role);

		case Column::Count:
            break;
    }

    return {};
}

void ProjectCenterModel::setSourceUrl(const QUrl& sourceUrl)
{
	_fileDownloader.download(sourceUrl);
}

QSet<QString> ProjectCenterModel::getTagsSet() const
{
    return _tags;
}

void ProjectCenterModel::addProject(const ProjectCenterProject* project)
{
    Q_ASSERT(project);

    if (!project)
        return;

    appendRow(Row(project));
    updateTags();

    const_cast<ProjectCenterProject*>(project)->setParent(this);

    _projects.push_back(project);
}

void ProjectCenterModel::updateTags()
{
    for (int rowIndex = 0; rowIndex < rowCount(); ++rowIndex)
        for (const auto& tag : dynamic_cast<Item*>(itemFromIndex(index(rowIndex, 0)))->getProject()->getTags())
            _tags.insert(tag);

    emit tagsChanged(_tags);
}

const ProjectCenterProject* ProjectCenterModel::getProject(const QModelIndex& index) const
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

const ProjectCenterProjects& ProjectCenterModel::getProjects() const
{
	return _projects;
}

ProjectCenterModel::Item::Item(const mv::util::ProjectCenterProject* project, bool editable /*= false*/) :
    _project(project)
{
}

const ProjectCenterProject* ProjectCenterModel::Item::getProject() const
{
    return _project;
}

QVariant ProjectCenterModel::TitleItem::data(int role /*= Qt::UserRole + 1*/) const
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

QVariant ProjectCenterModel::TagsItem::data(int role /*= Qt::UserRole + 1*/) const
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

QVariant ProjectCenterModel::DateItem::data(int role /*= Qt::UserRole + 1*/) const
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

QVariant ProjectCenterModel::IconNameItem::data(int role /*= Qt::UserRole + 1*/) const
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

QVariant ProjectCenterModel::SummaryItem::data(int role /*= Qt::UserRole + 1*/) const
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

QVariant ProjectCenterModel::UrlItem::data(int role) const
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

QVariant ProjectCenterModel::MinimumVersionMajorItem::data(int role) const
{
    switch (role) {
	    case Qt::EditRole:
            return getProject()->getMinimumVersionMajor();

		case Qt::DisplayRole:
	        return QString::number(data(Qt::EditRole).toInt());

	    case Qt::ToolTipRole:
	        return "Minimum ManiVault Studio application version (major): " + data(Qt::DisplayRole).toString();

	    default:
	        break;
    }
    
	return Item::data(role);
}

QVariant ProjectCenterModel::MinimumVersionMinorItem::data(int role) const
{
    switch (role) {
	    case Qt::EditRole:
	        return getProject()->getMinimumVersionMinor();

	    case Qt::DisplayRole:
	        return QString::number(data(Qt::EditRole).toInt());

	    case Qt::ToolTipRole:
	        return "Minimum ManiVault Studio application version (minor): " + data(Qt::DisplayRole).toString();

	    default:
	        break;
    }

	return Item::data(role);
}

QVariant ProjectCenterModel::RequiredPluginsItem::data(int role) const
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

QVariant ProjectCenterModel::MissingPluginsItem::data(int role) const
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
