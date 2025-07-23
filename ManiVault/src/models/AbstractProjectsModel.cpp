// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "AbstractProjectsModel.h"

#include "util/Miscellaneous.h"
#include "util/JSON.h"

#include <nlohmann/json.hpp>

#include <QJsonObject>
#include <QJsonArray>

#ifdef _DEBUG
    //#define ABSTRACT_PROJECTS_MODEL_VERBOSE
#endif

#define ABSTRACT_PROJECTS_MODEL_VERBOSE

using namespace mv::util;
using namespace mv::gui;

using nlohmann::json;

namespace mv {

AbstractProjectsModel::AbstractProjectsModel(const PopulationMode& populationMode /*= PopulationMode::Automatic*/, QObject* parent /*= nullptr*/) :
    StandardItemModel(parent, "Projects", populationMode),
    _dsnsAction(this, "Data Source Names"),
    _editDsnsAction(this, "Edit projects sources..."),
    _numberOfPopulators(0)
{
    setColumnCount(static_cast<int>(Column::Count));

    _dsnsAction.setIconByName("globe");
    _dsnsAction.setToolTip("Projects Data Source Names (DSN)");
    _dsnsAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _dsnsAction.setDefaultWidgetFlags(StringsAction::WidgetFlag::ListView);
    _dsnsAction.setPopupSizeHint(QSize(550, 100));
    _dsnsAction.setDefaultWidgetFlag(StringsAction::WidgetFlag::MayEdit);
    _dsnsAction.setCategory("Projects DSN");

    if (getPopulationMode() == PopulationMode::Automatic || getPopulationMode() == PopulationMode::AutomaticSynchronous) {

        connect(&getDsnsAction(), &StringsAction::stringsAdded, this, [this](const QStringList& addedStrings) -> void {
            for (const auto& addedString : addedStrings)
                addDsn(addedString);
        });

        connect(&getDsnsAction(), &StringsAction::stringsRemoved, this, [this](const QStringList& removedStrings) -> void {
            for (const auto& removedString : removedStrings)
                removeDsn(removedString);
        });

        connect(core(), &CoreInterface::initialized, this, [this]() -> void {
            for (auto pluginFactory : mv::plugins().getPluginFactoriesByTypes()) {
                getDsnsAction().addStrings(pluginFactory->getProjectsDsnsAction().getStrings());

                connect(&pluginFactory->getProjectsDsnsAction(), &StringsAction::stringsAdded, this, [this](const QStringList& addedStrings) -> void {
                    getDsnsAction().addStrings(addedStrings);
                });

                connect(&pluginFactory->getProjectsDsnsAction(), &StringsAction::stringsRemoved, this, [this](const QStringList& removedStrings) -> void {
                    getDsnsAction().removeStrings(removedStrings);
                });
            }
		});
    }

    _editDsnsAction.setIconByName("gear");

    connect(&_editDsnsAction, &TriggerAction::triggered, this, [this]() {
        QDialog editDsnsDialog;

        editDsnsDialog.setWindowIcon(StyledIcon("gear"));
        editDsnsDialog.setWindowTitle("Edit projects sources");
        editDsnsDialog.setMinimumWidth(800);
        editDsnsDialog.setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

        auto layout = new QVBoxLayout(&editDsnsDialog);

        layout->setSpacing(10);

        layout->addWidget(_dsnsAction.createWidget(&editDsnsDialog));

        auto dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok);

        connect(dialogButtonBox->button(QDialogButtonBox::StandardButton::Ok), &QPushButton::clicked, &editDsnsDialog, &QDialog::accept);

        layout->addWidget(dialogButtonBox);

        editDsnsDialog.setLayout(layout);
        editDsnsDialog.exec();
    });
}

QVariant AbstractProjectsModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    switch (static_cast<Column>(section))
    {
        case Column::Title:
            return TitleItem::headerData(orientation, role);

        case Column::LastModified:
            return LastModifiedItem::headerData(orientation, role);

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

        case Column::DownloadSize:
            return DownloadSizeItem::headerData(orientation, role);

        case Column::SystemCompatibility:
            return SystemCompatibilityItem::headerData(orientation, role);

        case Column::IsStartup:
            return IsStartupItem::headerData(orientation, role);

        case Column::Sha:
            return ShaItem::headerData(orientation, role);

		case Column::Count:
            break;
    }

    return {};
}

void AbstractProjectsModel::addProject(const ProjectsModelProjectPtr& project, const QModelIndex& parentIndex /*= QModelIndex()*/)
{
    try {
        Q_ASSERT(project);

        if (!project)
            throw std::runtime_error("Project pointer not valid");

#ifdef ABSTRACT_PROJECTS_MODEL_VERBOSE
        qDebug() << __FUNCTION__ << project->getTitle() << parentIndex;
#endif
        beginPopulate();
	    {
		    updateTags();

        	project->setParent(this);
        	project->updateMetadata();

        	_projects.push_back(project);

        	if (parentIndex.isValid()) {
        		if (auto parentItem = dynamic_cast<Item*>(itemFromIndex(parentIndex)))
        			parentItem->appendRow(Row(project.get()));
        		else
        			throw std::runtime_error("Parent index is not a valid item");
        	}
        	else {
        		appendRow(Row(project.get()));
        	}
	    }
        endPopulate();
    }
    catch (std::exception& e)
    {
        qCritical() << QString("Unable to add %1 to the projects model:").arg(project ? project->getTitle() : "untitled") << e.what();
    }
    catch (...)
    {
        qCritical() << QString("Unable to add %1 to the projects model due to an unhandled exception").arg(project ? project->getTitle() : "untitled");
    }
}

void AbstractProjectsModel::removeProject(const util::ProjectsModelProjectPtr& project)
{
    try {
#ifdef ABSTRACT_PROJECTS_MODEL_VERBOSE
        qDebug() << __FUNCTION__ << project->getTitle();
#endif

        beginPopulate();
        {
			const auto modelIndices = match(index(0, static_cast<std::int32_t>(Column::Title)), Qt::DisplayRole, project->getTitle(), -1, Qt::MatchExactly | Qt::MatchRecursive);

			for (const auto& modelIndex : modelIndices)
				removeRow(modelIndex.siblingAtColumn(0).row(), modelIndex.parent());

			std::erase(_projects, project);
        }
        endPopulate();
    }
    catch (std::exception& e)
    {
        qCritical() << QString("Unable to remove %1 from the projects tree model:").arg(project->getTitle()) << e.what();
    }
    catch (...)
    {
        qCritical() << QString("Unable to remove %1 from the projects tree model due to an unhandled exception").arg(project->getTitle());
    }
}

void AbstractProjectsModel::addDsn(const QUrl& dsn)
{
    try {
#ifdef ABSTRACT_PROJECTS_MODEL_VERBOSE
        qDebug() << __FUNCTION__ << dsn;
#endif

        if (getPopulationMode() == PopulationMode::Automatic) {
            for (const auto& dsn : getDsnsAction().getStrings()) {
                const auto dsnIndex = getDsnsAction().getStrings().indexOf(dsn);

                FileDownloader::downloadToByteArrayAsync(dsn)
                    .then(this, [this, dsn, dsnIndex](const QByteArray& data) {
	                    try {
                            for (const auto& project : _projects)
                                if (project->getProjectsJsonDsn() == dsn)
                                    removeProject(project);

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
                    .onFailed(this, [this, dsn](const QException& e) {
						qWarning().noquote() << "Download failed for" << dsn << ":" << e.what();
					});
            }
        }

        if (getPopulationMode() == PopulationMode::AutomaticSynchronous) {
            for (const auto& dsn : getDsnsAction().getStrings()) {
                try {
                    const auto data = FileDownloader::downloadToByteArraySync(dsn);

                    populateFromJsonByteArray(data, getDsnsAction().getStrings().indexOf(dsn), dsn);
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
        }
    }
    catch (std::exception& e)
    {
        qCritical() << QString("Unable to add %1 to the projects tree model:").arg(dsn.toDisplayString()) << e.what();
    }
    catch (...)
    {
        qCritical() << QString("Unable to add %1 to the projects tree model due to an unhandled exception").arg(dsn.toDisplayString());
    }
}

void AbstractProjectsModel::removeDsn(const QUrl& dsn)
{
    try {
#ifdef ABSTRACT_PROJECTS_MODEL_VERBOSE
    qDebug() << __FUNCTION__ << dsn;
#endif

		beginPopulate();
	    {
		    for (const auto& project : _projects)
		    	if (project->getProjectsJsonDsn() == dsn)
		    		removeProject(project);
	    }
        endPopulate();
    }
    catch (std::exception& e)
    {
        qCritical() << QString("Unable to add %1 to the projects tree model:").arg(dsn.toDisplayString()) << e.what();
    }
    catch (...)
    {
        qCritical() << QString("Unable to add %1 to the projects tree model due to an unhandled exception").arg(dsn.toDisplayString());
    }
}

void AbstractProjectsModel::populateFromJsonByteArray(const QByteArray& jsonByteArray, std::int32_t dsnIndex, const QString& jsonLocation)
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

        ProjectsModelProjectPtrs projects;

        for (const auto project : jsonDocument.object()["projects"].toArray()) {
            auto projectMap = project.toVariant().toMap();

            projectMap["projectsJsonDsn"] = QUrl(jsonLocation);

            projects.push_back(std::make_shared<ProjectsModelProject>(projectMap));
        }

        beginPopulate();
	    {
		    populate(projects);
	    }
        endPopulate();
    }
    catch (std::exception& e)
    {
        qCritical() << "Unable to populate projects model from JSON content:" << e.what();
    }
    catch (...)
    {
        qCritical() << "Unable to populate projects model from JSON content due to an unhandled exception";
    }
}

void AbstractProjectsModel::populateFromJsonFile(const QString& filePath)
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

void AbstractProjectsModel::updateTags()
{
    for (int rowIndex = 0; rowIndex < rowCount(); ++rowIndex)
        for (const auto& tag : dynamic_cast<Item*>(itemFromIndex(index(rowIndex, 0)))->getProject()->getTags())
            _tags.insert(tag);

    emit tagsChanged(_tags);
}

QSet<QString> AbstractProjectsModel::getTagsSet() const
{
    return _tags;
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

const ProjectsModelProjectPtrs& AbstractProjectsModel::getProjects() const
{
    return _projects;
}

void AbstractProjectsModel::beginPopulate()
{
    _numberOfPopulators++;

    emit aboutToBePopulated();
}

void AbstractProjectsModel::populate(const util::ProjectsModelProjectPtrs& projects)
{
    removeRows(0, rowCount());
}

void AbstractProjectsModel::endPopulate()
{
    _numberOfPopulators--;

    if (_numberOfPopulators == 0)
		emit populated();
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
            return QString("%1 (double-click to load)").arg(data(Qt::DisplayRole).toString());

		case Qt::DecorationRole:
            if (getProject()->isGroup())
                return StyledIcon("folder");

            return getProject()->isDownloaded() ? StyledIcon(getProject()->getIconName()) : StyledIcon("download");

        default:
            break;
    }

    return Item::data(role);
}

AbstractProjectsModel::LastModifiedItem::LastModifiedItem(const util::ProjectsModelProject* project, bool editable) :
    Item(project, editable)
{
    Q_ASSERT(project);

    if (project) {
        connect(project, &ProjectsModelProject::lastModifiedDetermined, this, [this](const QDateTime& lastModified) {
            emitDataChanged();
        });
    }
}

QVariant AbstractProjectsModel::LastModifiedItem::data(int role) const
{
    switch (role) {
	    case Qt::EditRole:
	        return getProject()->getLastModified();

	    case Qt::DisplayRole:
	        return data(Qt::EditRole).toDateTime().toString();

	    case Qt::ToolTipRole:
	        return "Last modified: " + data(Qt::DisplayRole).toString();

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
	        return data(Qt::EditRole).toStringList().join(", ");

	    case Qt::ToolTipRole:
	        return "Required plugins: " + data(Qt::DisplayRole).toString();

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
	        return data(Qt::EditRole).toStringList().join(", ");

	    case Qt::ToolTipRole:
	        return "Missing plugins: " + data(Qt::DisplayRole).toString();

	    default:
	        break;
    }

    return Item::data(role);
}

AbstractProjectsModel::DownloadSizeItem::DownloadSizeItem(const util::ProjectsModelProject* project, bool editable) :
    Item(project, editable)
{
    Q_ASSERT(project);

    if (project) {
	    connect(project, &ProjectsModelProject::downloadSizeDetermined, this, [this](std::uint64_t size) {
            emitDataChanged();
        });
    }
}

QVariant AbstractProjectsModel::DownloadSizeItem::data(int role) const
{
    switch (role) {
		case Qt::EditRole:
            return QVariant::fromValue(getProject()->getDownloadSize());

		case Qt::DisplayRole:
            return getNoBytesHumanReadable(data(Qt::EditRole).toULongLong());

	    case Qt::ToolTipRole:
	        return "Download size: " + data(Qt::DisplayRole).toString();

	    default:
	        break;
    }

	return Item::data(role);
}

QVariant AbstractProjectsModel::SystemCompatibilityItem::data(int role) const
{
    static constexpr auto notCompatibleColor    = QColor(255, 70, 70);
    static constexpr auto notRecommendedColor   = QColor(255, 127, 39);

    switch (role) {
		case Qt::EditRole:
	        return QVariant::fromValue(HardwareSpec::getSystemCompatibility(getProject()->getMinimumHardwareSpec(), getProject()->getRecommendedHardwareSpec()));

	    case Qt::DisplayRole:
        {
            switch (data(Qt::EditRole).value<HardwareSpec::SystemCompatibilityInfo>()._compatibility) {
	            case HardwareSpec::SystemCompatibility::Incompatible:
	                return "Unsupported hardware";

	            case HardwareSpec::SystemCompatibility::Minimum:
	                return "Not recommended";

	            case HardwareSpec::SystemCompatibility::Compatible:
	                return "Compatible";

	            case HardwareSpec::SystemCompatibility::Unknown:
                    return "Unknown";
            }

            break;
        }

        case Qt::ToolTipRole:
            return QString("<div style='min-width: 400px;'>%1</div>").arg(data(Qt::EditRole).value<HardwareSpec::SystemCompatibilityInfo>()._message);

        case Qt::DecorationRole:
        {
            const auto systemCompatibility = HardwareSpec::getSystemCompatibility(getProject()->getMinimumHardwareSpec(), getProject()->getRecommendedHardwareSpec());

	        switch (data(Qt::EditRole).value<HardwareSpec::SystemCompatibilityInfo>()._compatibility) {
				case HardwareSpec::SystemCompatibility::Incompatible:
                    return StyledIcon("circle-xmark").withColor(notCompatibleColor);

                case HardwareSpec::SystemCompatibility::Minimum:
                    return StyledIcon("circle-exclamation").withColor(notRecommendedColor);

                case HardwareSpec::SystemCompatibility::Compatible:
                    return StyledIcon("circle-check");

		        case HardwareSpec::SystemCompatibility::Unknown:
                    return StyledIcon("circle-question");
	        }

            break;
        }

		case Qt::ForegroundRole:
	    {
            const auto systemCompatibility  = HardwareSpec::getSystemCompatibility(getProject()->getMinimumHardwareSpec(), getProject()->getRecommendedHardwareSpec());
            const auto notCompatible        = systemCompatibility._compatibility == HardwareSpec::SystemCompatibility::Incompatible;
            const auto notRecommended       = systemCompatibility._compatibility == HardwareSpec::SystemCompatibility::Minimum;

            if (notCompatible)
                return notCompatibleColor;

            if (notRecommended)
                return QBrush(notRecommendedColor);

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

QVariant AbstractProjectsModel::ShaItem::data(int role) const
{
    switch (role) {
	    case Qt::EditRole:
		case Qt::DisplayRole:
            return getProject()->getSha();

	    case Qt::ToolTipRole:
	        return "SHA: " + data(Qt::DisplayRole).toString();

	    default:
	        break;
    }

    return Item::data(role);
}

QVariant AbstractProjectsModel::ProjectsJsonDsnItem::data(int role) const
{
    switch (role) {
	    case Qt::EditRole:
	        return getProject()->getProjectsJsonDsn();

	    case Qt::DisplayRole:
	        return data(Qt::EditRole).toString();

	    case Qt::ToolTipRole:
	        return "Projects JSON file DSN: " + data(Qt::DisplayRole).toString();

	    default:
	        break;
    }

    return Item::data(role);
}

}
