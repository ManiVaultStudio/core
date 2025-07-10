// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "AbstractProjectsModel.h"

#ifdef _DEBUG
    //#define ABSTRACT_PROJECTS_MODEL_VERBOSE
#endif

using namespace mv::util;
using namespace mv::gui;

namespace mv {

//QMap<AbstractProjectsModel::Column, AbstractProjectsModel::ColumHeaderInfo> AbstractProjectsModel::columnInfo = QMap<Column, ColumHeaderInfo>({
//    { Column::Title, { "Title" , "Title", "Title" } },
//    { Column::Downloaded, { "Downloaded" , "Downloaded", "Whether the project has been downloaded before" } },
//    { Column::Group, { "Group" , "Group", "Group" } },
//    { Column::IsGroup, { "IsGroup" , "IsGroup", "IsGroup" } },
//    { Column::Tags, { "Tags" , "Tags", "Tags" } },
//    { Column::Date, { "Date" , "Date", "Issue date" } },
//    { Column::IconName, { "Icon Name" , "Icon Name", "Font Awesome icon name" } },
//    { Column::Summary, { "Summary" , "Summary", "Summary (brief description)" } },
//    { Column::Url, { "URL" , "URL", "Project URL" } },
//    { Column::MinimumCoreVersion, { "Min. app core version" , "Min. app core version", "Minimum ManiVault Studio application core version" } },
//    { Column::RequiredPlugins, { "Required plugins" , "Required plugins", "Plugins required to open the project" } },
//    { Column::MissingPlugins, { "Missing plugins" , "Missing plugins", "List of plugins which are missing" } },
//    { Column::Size, { "Size" , "Size", "Project size" } },
//    { Column::SystemCompatibility, { "SystemCompatibility" , "SystemCompatibility", "Minimum hardware specification for opening the project" } },
//    { Column::IsStartup, { "Startup" , "Startup", "Whether this is a startup project" } }
//});

AbstractProjectsModel::AbstractProjectsModel(const PopulationMode& populationMode /*= PopulationMode::Automatic*/, QObject* parent /*= nullptr*/) :
    StandardItemModel(parent, "Projects", populationMode)
{
    setColumnCount(static_cast<int>(Column::Count));
}

QVariant AbstractProjectsModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    switch (static_cast<Column>(section))
    {
        case Column::Title:
            return TitleItem::headerData(orientation, role);

        case Column::Downloaded:
            return DownloadedItem::headerData(orientation, role);

        case Column::Group:
            return GroupItem::headerData(orientation, role);

        case Column::IsGroup:
            return IsGroupItem::headerData(orientation, role);

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

        case Column::Size:
            return SizeItem::headerData(orientation, role);

        case Column::SystemCompatibility:
            return SystemCompatibilityItem::headerData(orientation, role);

        case Column::IsStartup:
            return IsStartupItem::headerData(orientation, role);

		case Column::Count:
            break;
    }

    return {};
}

QSet<QString> AbstractProjectsModel::getTagsSet() const
{
    return _tags;
}

void AbstractProjectsModel::addProjectGroup(const QString& groupTitle)
{
    Q_ASSERT(!groupTitle.isEmpty());

    if (groupTitle.isEmpty())
        return;

    auto project = new ProjectsModelProject(groupTitle);

    appendRow(Row(project));
    updateTags();

    const_cast<ProjectsModelProject*>(project)->setParent(this);

    _projects.push_back(project);
}

void AbstractProjectsModel::addProject(const ProjectsModelProject* project, const QString& groupTitle)
{
    Q_ASSERT(project);

    if (!project)
        return;

    const auto findProjectGroupModelIndex = [this, &groupTitle]() -> QModelIndex {
        const auto matches = match(index(0, static_cast<std::int32_t>(Column::Title)), Qt::DisplayRole, groupTitle, -1, Qt::MatchExactly | Qt::MatchRecursive);

        if (matches.size() == 1)
            return matches.first().siblingAtColumn(static_cast<std::int32_t>(Column::Title));

        return {};
    };

    if (!groupTitle.isEmpty()) {
        const auto existingProjectGroupIndex = findProjectGroupModelIndex();

        if (existingProjectGroupIndex.isValid()) {
            if (auto existingProjectGroupItem = itemFromIndex(existingProjectGroupIndex)) {
                existingProjectGroupItem->appendRow(Row(project));
            }
        } else {
            addProjectGroup(groupTitle);

            const auto addedProjectGroupIndex = findProjectGroupModelIndex();

            if (addedProjectGroupIndex.isValid())
                if (auto addedProjectGroupItem = itemFromIndex(addedProjectGroupIndex))
                    addedProjectGroupItem->appendRow(Row(project));
        }
    }
    else
    {
        appendRow(Row(project));
    }
    
    updateTags();

    const_cast<ProjectsModelProject*>(project)->setParent(this);

    _projects.push_back(project);
}

void AbstractProjectsModel::updateTags()
{
    for (int rowIndex = 0; rowIndex < rowCount(); ++rowIndex)
        for (const auto& tag : dynamic_cast<Item*>(itemFromIndex(index(rowIndex, 0)))->getProject()->getTags())
            _tags.insert(tag);

    emit tagsChanged(_tags);
}

const ProjectsModelProject* AbstractProjectsModel::getProject(const QModelIndex& index) const
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

const ProjectDatabaseProjects& AbstractProjectsModel::getProjects() const
{
	return _projects;
}

AbstractProjectsModel::HardwareSpecTooltipPopup::HardwareSpecTooltipPopup(QWidget* parent): QFrame(parent, Qt::ToolTip)
{
	setWindowFlags(Qt::ToolTip);
	setFrameShape(QFrame::Box);
	setStyleSheet("background: white; border: 1px solid gray;");

	auto* layout = new QVBoxLayout(this);
	_label       = new QLabel("Details here...");
	layout->addWidget(_label);
}

void AbstractProjectsModel::HardwareSpecTooltipPopup::setText(const QString& text)
{
	_label->setText(text);
}

AbstractProjectsModel::Item::Item(const mv::util::ProjectsModelProject* project, bool editable /*= false*/) :
    _project(project)
{
    setEditable(editable);
}

const ProjectsModelProject* AbstractProjectsModel::Item::getProject() const
{
    return _project;
}

QVariant AbstractProjectsModel::TitleItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getProject()->getTitle();

        case Qt::ToolTipRole:
            return "Title: " + data(Qt::DisplayRole).toString();

		case Qt::DecorationRole:
            if (getProject()->isGroup())
                return StyledIcon("folder");

            return getProject()->isDownloaded() ? StyledIcon(getProject()->getIconName()) : StyledIcon("download");

        default:
            break;
    }

    return Item::data(role);
}

AbstractProjectsModel::DownloadedItem::DownloadedItem(const util::ProjectsModelProject* project, bool editable) :
    Item(project, editable)
{
}

QVariant AbstractProjectsModel::DownloadedItem::data(int role) const
{
    switch (role) {
	    case Qt::EditRole:
            return getProject()->isDownloaded();

	    case Qt::DisplayRole:
	        return "";

	    case Qt::ToolTipRole:
	        return data(Qt::EditRole).toBool() ? "Project is downloaded before" : "Not downloaded yet";

	    case Qt::DecorationRole:
	        return data(Qt::EditRole).toBool() ? StyledIcon("file") : StyledIcon("download");

	    default:
	        break;
    }

	return Item::data(role);
}

QVariant AbstractProjectsModel::GroupItem::data(int role) const
{
    switch (role) {
	    case Qt::EditRole:
	    case Qt::DisplayRole:
	        return getProject()->getGroup();

	    case Qt::ToolTipRole:
	        return "Group title: " + data(Qt::DisplayRole).toString();

	    default:
	        break;
    }

    return Item::data(role);
}

QVariant AbstractProjectsModel::IsGroupItem::data(int role) const
{
    switch (role) {
	    case Qt::EditRole:
            return getProject()->isGroup();

	    case Qt::DisplayRole:
	        return getProject()->isGroup() ? "true" : "false";

	    case Qt::ToolTipRole:
	        return "Is group: " + data(Qt::DisplayRole).toString();

	    default:
	        break;
    }

    return Item::data(role);
}

QVariant AbstractProjectsModel::TagsItem::data(int role /*= Qt::UserRole + 1*/) const
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

QVariant AbstractProjectsModel::DateItem::data(int role /*= Qt::UserRole + 1*/) const
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

QVariant AbstractProjectsModel::IconNameItem::data(int role /*= Qt::UserRole + 1*/) const
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

QVariant AbstractProjectsModel::SummaryItem::data(int role /*= Qt::UserRole + 1*/) const
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

QVariant AbstractProjectsModel::UrlItem::data(int role) const
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

QVariant AbstractProjectsModel::MinimumCoreVersionItem::data(int role) const
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

QVariant AbstractProjectsModel::RequiredPluginsItem::data(int role) const
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

QVariant AbstractProjectsModel::MissingPluginsItem::data(int role) const
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

QVariant AbstractProjectsModel::SizeItem::data(int role) const
{
    switch (role) {
	    case Qt::EditRole:
	    case Qt::DisplayRole:
            return getProject()->getSize();

	    case Qt::ToolTipRole:
	        return "Project size: " + data(Qt::DisplayRole).toString();

	    default:
	        break;
    }

	return Item::data(role);
}

QVariant AbstractProjectsModel::SystemCompatibilityItem::data(int role) const
{
    switch (role) {
    case Qt::EditRole:
	        return QVariant::fromValue(HardwareSpec::getSystemCompatibility(getProject()->getMinimumHardwareSpec(), getProject()->getRecommendedHardwareSpec()));

	    case Qt::DisplayRole:
        {
            if (data(Qt::EditRole).value<HardwareSpec::SystemCompatibilityInfo>()._compatibility == HardwareSpec::SystemCompatibility::Unknown)
                return "Not available";

            return "";
        }

        case Qt::ToolTipRole:
            return QString("<div style='width: 600px;'>%1</div>").arg(data(Qt::EditRole).value<HardwareSpec::SystemCompatibilityInfo>()._failureString);

        case Qt::DecorationRole:
        {
	        switch (data(Qt::EditRole).value<HardwareSpec::SystemCompatibilityInfo>()._compatibility) {
				case HardwareSpec::SystemCompatibility::Incompatible:
                    return StyledIcon("circle-xmark").withColor(Qt::darkRed);

                case HardwareSpec::SystemCompatibility::Minimum:
                    return StyledIcon("circle-exclamation").withColor(Qt::darkYellow);

                case HardwareSpec::SystemCompatibility::Compatible:
                    return StyledIcon("circle-check");

		        case HardwareSpec::SystemCompatibility::Unknown:
                    break;
	        }

            break;
        }

	    default:
	        break;
    }

    return Item::data(role);
}

QVariant AbstractProjectsModel::IsStartupItem::data(int role) const
{
    switch (role) {
	    case Qt::EditRole:
	        return getProject()->isStartup();

	    case Qt::DisplayRole:
	        return data(Qt::EditRole).toBool() ? "true" : "false";

	    case Qt::ToolTipRole:
	        return "IsStartup project: " + data(Qt::DisplayRole).toString();

	    default:
	        break;
    }

	return Item::data(role);
}

}
