// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LearningCenterTutorialsModel.h"

#include "util/JSON.h"

#include <QtConcurrent>

#ifdef _DEBUG
    //#define LEARNING_CENTER_TUTORIALS_MODEL_VERBOSE
#endif

using nlohmann::json;
using nlohmann::json_schema::json_validator;

using namespace mv::util;
using namespace mv::gui;

namespace mv {

QMap<LearningCenterTutorialsModel::Column, LearningCenterTutorialsModel::ColumHeaderInfo> LearningCenterTutorialsModel::columnInfo = QMap<Column, ColumHeaderInfo>({
    { Column::Title, { "Title" , "Title", "Title" } },
    { Column::Tags, { "Tags" , "Tags", "Tags" } },
    { Column::Date, { "Date" , "Date", "Issue date" } },
    { Column::IconName, { "Icon Name" , "Icon Name", "Font Awesome icon name" } },
    { Column::Summary, { "Summary" , "Summary", "Summary (brief description)" } },
    { Column::Content, { "Content" , "Content", "Full tutorial content in HTML format" } },
    { Column::Url, { "URL" , "URL", "ManiVault website tutorial URL" } },
    { Column::MinimumCoreVersion, { "Min. app core version" , "Min. app core version", "Minimum ManiVault Studio application core version" } },
    { Column::RequiredPlugins, { "Required plugins" , "Required plugins", "Plugins required to open the project" } },
    { Column::MissingPlugins, { "Missing plugins" , "Missing plugins", "List of plugins which are missing" } },
});

LearningCenterTutorialsModel::LearningCenterTutorialsModel(QObject* parent /*= nullptr*/) :
    QStandardItemModel(parent),
    _dsnsAction(this, "Data Source Names")
{
    setColumnCount(static_cast<int>(Column::Count));

    _dsnsAction.setIconByName("globe");
    _dsnsAction.setToolTip("Tutorials Data Source Names (DSN)");
    _dsnsAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _dsnsAction.setDefaultWidgetFlags(StringsAction::WidgetFlag::ListView);
    _dsnsAction.setPopupSizeHint(QSize(550, 100));

    connect(&_dsnsAction, &StringsAction::stringsChanged, this, [this]() -> void {
        setRowCount(0);

        _future = QtConcurrent::mapped(
            _dsnsAction.getStrings(),
            [this](const QString& dsn) {
                return downloadTutorialsFromDsn(dsn);
		});

        connect(&_watcher, &QFutureWatcher<QByteArray>::finished, [&]() {
            for (int dsnIndex = 0; dsnIndex < _dsnsAction.getStrings().size(); ++dsnIndex) {
                try {
                    const auto jsonData = _future.resultAt<QByteArray>(dsnIndex);

                    json fullJson = json::parse(jsonData.constData());

                    if (fullJson.contains("tutorials")) {
                        validateJsonWithResourceSchema(fullJson["tutorials"], _dsnsAction.getStrings()[dsnIndex].toStdString(), ":/JSON/TutorialsSchema", "https://github.com/ManiVaultStudio/core/tree/master/ManiVault/res/json/TutorialsSchema.json");
                    }
                    else {
                        throw std::runtime_error("Tutorials key is missing");
                    }

                    QJsonParseError jsonParseError;

                    const auto jsonDocument = QJsonDocument::fromJson(jsonData, &jsonParseError);

                    if (jsonParseError.error != QJsonParseError::NoError || !jsonDocument.isObject()) {
                        qCritical() << "Invalid JSON from DSN at index" << dsnIndex << ":" << jsonParseError.errorString();
                        continue;
                    }

                    for (const auto tutorial : jsonDocument.object()["tutorials"].toArray())
                        addTutorial(new LearningCenterTutorial(tutorial.toVariant().toMap()));
                }
                catch (std::exception& e)
                {
                    qCritical() << "Unable to add tutorials from DSN:" << e.what();
                }
                catch (...)
                {
                    qCritical() << "Unable to add tutorials from DSN due to an unhandled exception";
                }
            }

            emit populatedFromDsns();
        });

        _watcher.setFuture(_future);
	});

    for (auto pluginFactory : mv::plugins().getPluginFactoriesByTypes()) {
        connect(&pluginFactory->getTutorialsDsnsAction(), &StringsAction::stringsChanged, this, &LearningCenterTutorialsModel::synchronizeWithDsns);
    }
}

QVariant LearningCenterTutorialsModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
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

        case Column::Content:
            return ContentItem::headerData(orientation, role);

        case Column::Url:
            return UrlItem::headerData(orientation, role);

        case Column::ProjectUrl:
            return ProjectUrlItem::headerData(orientation, role);

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

QSet<QString> LearningCenterTutorialsModel::getTagsSet() const
{
    return _tags;
}

void LearningCenterTutorialsModel::addTutorial(const LearningCenterTutorial* tutorial)
{
    Q_ASSERT(tutorial);

    if (!tutorial)
        return;

    appendRow(Row(tutorial));
    updateTags();

    const_cast<LearningCenterTutorial*>(tutorial)->setParent(this);

    _tutorials.push_back(tutorial);
}

void LearningCenterTutorialsModel::updateTags()
{
    for (int rowIndex = 0; rowIndex < rowCount(); ++rowIndex)
        for (const auto& tag : dynamic_cast<Item*>(itemFromIndex(index(rowIndex, 0)))->getTutorial()->getTags())
            _tags.insert(tag);

    emit tagsChanged(_tags);
}

void LearningCenterTutorialsModel::synchronizeWithDsns()
{
    auto uniqueDsns = _dsnsAction.getStrings();

    for (auto pluginFactory : mv::plugins().getPluginFactoriesByTypes()) {
        uniqueDsns << pluginFactory->getTutorialsDsnsAction().getStrings();
    }

    uniqueDsns.removeDuplicates();

    _dsnsAction.setStrings(uniqueDsns);
}

QByteArray LearningCenterTutorialsModel::downloadTutorialsFromDsn(const QString& dsn)
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
            exceptionMessageBox("Unable to download tutorials JSON from DSN", e);
        }
        catch (...)
        {
            exceptionMessageBox("Unable to download tutorials JSON from DSN");
        }
    });

    fileDownloader.download(dsn);

    loop.exec();

    return downloadedData;
}

LearningCenterTutorialsModel::Item::Item(const mv::util::LearningCenterTutorial* tutorial, bool editable /*= false*/) :
    _tutorial(tutorial)
{
}

const LearningCenterTutorial* LearningCenterTutorialsModel::Item::getTutorial() const
{
    return _tutorial;
}

QVariant LearningCenterTutorialsModel::TitleItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getTutorial()->getTitle();

        case Qt::ToolTipRole:
            return "Title: " + data(Qt::DisplayRole).toString();

    case Qt::DecorationRole:
            return StyledIcon(getTutorial()->getIconName());

        default:
            break;
    }

    return Item::data(role);
}

QVariant LearningCenterTutorialsModel::TagsItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return getTutorial()->getTags();

        case Qt::DisplayRole:
            return data(Qt::EditRole).toStringList().join(",");

        case Qt::ToolTipRole:
            return "Tags: " + data(Qt::DisplayRole).toStringList().join(",");

        default:
            break;
    }

    return Item::data(role);
}

QVariant LearningCenterTutorialsModel::DateItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getTutorial()->getDate();

        case Qt::ToolTipRole:
            return "Date: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

QVariant LearningCenterTutorialsModel::IconNameItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
	    case Qt::EditRole:
	    case Qt::DisplayRole:
	        return getTutorial()->getIconName();

	    case Qt::ToolTipRole:
	        return "Icon name: " + data(Qt::DisplayRole).toString();

	    default:
	        break;
    }

    return Item::data(role);
}

QVariant LearningCenterTutorialsModel::SummaryItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getTutorial()->getSummary();

        case Qt::ToolTipRole:
            return "Summary: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

QVariant LearningCenterTutorialsModel::ContentItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getTutorial()->getContent();

        case Qt::ToolTipRole:
            return "Content: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

QVariant LearningCenterTutorialsModel::UrlItem::data(int role) const
{
    switch (role) {
	    case Qt::EditRole:
            return QVariant::fromValue(getTutorial()->getUrl());

	    case Qt::DisplayRole:
            return QVariant::fromValue(getTutorial()->getUrl().toString());

        case Qt::ToolTipRole:
            return "URL: " + data(Qt::DisplayRole).toString();

	    default:
	        break;
    }

    return Item::data(role);
}

QVariant LearningCenterTutorialsModel::ProjectUrlItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return QVariant::fromValue(getTutorial()->getProjectUrl());

        case Qt::DisplayRole:
            return QVariant::fromValue(data(Qt::EditRole).toString());

        case Qt::ToolTipRole:
            return "Project URL: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

QVariant LearningCenterTutorialsModel::MinimumCoreVersionItem::data(int role) const
{
    switch (role) {
	    case Qt::EditRole:
	        return QVariant::fromValue(getTutorial()->getMinimumCoreVersion());

	    case Qt::DisplayRole:
	        return QString::fromStdString(data(Qt::EditRole).value<Version>().getVersionString());

	    case Qt::ToolTipRole:
	        return "Minimum ManiVault Studio application core version: " + data(Qt::DisplayRole).toString();

	    default:
	        break;
    }

    return Item::data(role);
}

QVariant LearningCenterTutorialsModel::RequiredPluginsItem::data(int role) const
{
    switch (role) {
    case Qt::EditRole:
        return getTutorial()->getRequiredPlugins();

    case Qt::DisplayRole:
        return data(Qt::EditRole).toStringList();

    case Qt::ToolTipRole:
        return "Required plugins: " + data(Qt::DisplayRole).toStringList().join(", ");

    default:
        break;
    }

    return Item::data(role);
}

QVariant LearningCenterTutorialsModel::MissingPluginsItem::data(int role) const
{
    switch (role) {
    case Qt::EditRole:
        return getTutorial()->getMissingPlugins();

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
