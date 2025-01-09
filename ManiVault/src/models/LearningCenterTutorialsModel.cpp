// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LearningCenterTutorialsModel.h"

#include "Application.h"

#ifdef _DEBUG
    //#define LEARNING_CENTER_TUTORIALS_MODEL_VERBOSE
#endif

using namespace mv::util;

namespace mv {

QMap<LearningCenterTutorialsModel::Column, LearningCenterTutorialsModel::ColumHeaderInfo> LearningCenterTutorialsModel::columnInfo = QMap<Column, ColumHeaderInfo>({
    { Column::Title, { "Title" , "Title", "Title" } },
    { Column::Tags, { "Tags" , "Tags", "Tags" } },
    { Column::Date, { "Date" , "Date", "Issue date" } },
    { Column::IconName, { "Icon Name" , "Icon Name", "Font Awesome icon name" } },
    { Column::Summary, { "Summary" , "Summary", "Summary (brief description)" } },
    { Column::Content, { "Content" , "Content", "Full tutorial content in HTML format" } },
    { Column::Url, { "URL" , "URL", "ManiVault website tutorial URL" } },
    { Column::MinimumVersionMajor, { "Min. app version (major)" , "Min. app version (major)", "Minimum ManiVault Studio application version (major)" } },
    { Column::MinimumVersionMinor, { "Min. app version (minor)" , "Min. app version (minor)", "Minimum ManiVault Studio application version (minor)" } }
});

LearningCenterTutorialsModel::LearningCenterTutorialsModel(QObject* parent /*= nullptr*/) :
    QStandardItemModel(parent)
{
    setColumnCount(static_cast<int>(Column::Count));
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

        case Column::MinimumVersionMajor:
            return MinimumVersionMajorItem::headerData(orientation, role);

        case Column::MinimumVersionMinor:
            return MinimumVersionMinorItem::headerData(orientation, role);

        default:
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
            return Application::getIconFont("FontAwesome").getIcon(getTutorial()->getIconName());

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

QVariant LearningCenterTutorialsModel::MinimumVersionMajorItem::data(int role) const
{
    switch (role) {
	    case Qt::EditRole:
            return getTutorial()->getMinimumVersionMajor();

		case Qt::DisplayRole:
	        return QString::number(data(Qt::EditRole).toInt());

	    case Qt::ToolTipRole:
	        return "Minimum ManiVault Studio application version (major): " + data(Qt::DisplayRole).toString();

	    default:
	        break;
    }
    
	return Item::data(role);
}

QVariant LearningCenterTutorialsModel::MinimumVersionMinorItem::data(int role) const
{
    switch (role) {
	    case Qt::EditRole:
	        return getTutorial()->getMinimumVersionMinor();

	    case Qt::DisplayRole:
	        return QString::number(data(Qt::EditRole).toInt());

	    case Qt::ToolTipRole:
	        return "Minimum ManiVault Studio application version (minor): " + data(Qt::DisplayRole).toString();

	    default:
	        break;
    }

	return Item::data(role);
}

}
