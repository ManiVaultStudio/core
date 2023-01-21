#include "StartPageActionsModel.h"

#ifdef _DEBUG
    //#define START_PAGE_ACTIONS_MODEL_VERBOSE
#endif

QMap<StartPageActionsModel::Column, QPair<QString, QString>> StartPageActionsModel::columns = QMap<StartPageActionsModel::Column, QPair<QString, QString>>({
    { StartPageActionsModel::Column::Icon, { "Icon", "Icon" }},
    { StartPageActionsModel::Column::Title, { "Title", "Title" }},
    { StartPageActionsModel::Column::Description, { "Description", "Description" }},
    { StartPageActionsModel::Column::Comments, { "Comments", "Comments" }},
    { StartPageActionsModel::Column::Tags, { "Tags", "Tags" }},
    { StartPageActionsModel::Column::PreviewImage, { "Preview", "Preview image" }},
    { StartPageActionsModel::Column::Tooltip, { "Tooltip", "Tooltip" }},
    { StartPageActionsModel::Column::ClickedCallback, { "Clicked Callback", "Callback which is called when the action is clicked" }},
    { StartPageActionsModel::Column::SummaryDelegate, { "Summary", "Delegate item with title and subtitle" }}
});

StartPageActionsModel::StartPageActionsModel(QObject* parent /*= nullptr*/) :
    QStandardItemModel(parent)
{
    for (const auto& column : columns.keys()) {
        auto columnItem = new QStandardItem(columns[column].first);
        
        columnItem->setToolTip(columns[column].second);

        setHorizontalHeaderItem(static_cast<int>(column), columnItem);
    }
}

void StartPageActionsModel::add(const StartPageAction& startPageAction)
{
#ifdef START_PAGE_ACTIONS_MODEL_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    QList<QStandardItem*> startPageActionRow = {
        new QStandardItem(),
        new QStandardItem(startPageAction.getTitle()),
        new QStandardItem(startPageAction.getDescription()),
        new QStandardItem(startPageAction.getComments()),
        new QStandardItem(),
        new QStandardItem(),
        new QStandardItem(startPageAction.getTooltip()),
        new QStandardItem(),
        new QStandardItem()
    };

    startPageActionRow[static_cast<int>(Column::Icon)]->setData(QVariant::fromValue(startPageAction.getIcon()));
    startPageActionRow[static_cast<int>(Column::Tags)]->setData(QVariant::fromValue(startPageAction.getTags()), Qt::EditRole);
    startPageActionRow[static_cast<int>(Column::PreviewImage)]->setData(QVariant::fromValue(startPageAction.getPreviewImage()));
    
    startPageActionRow[static_cast<int>(Column::ClickedCallback)]->setData(QVariant::fromValue(startPageAction.getClickedCallback()));

    for (auto item : startPageActionRow)
        item->setEditable(false);

    appendRow(startPageActionRow);

    emit layoutChanged();
}

bool StartPageActionsModel::hasComments() const
{
    for (int rowIndex = 0; rowIndex < rowCount(); rowIndex++)
        if (!data(index(rowIndex, static_cast<int>(Column::Comments))).toString().isEmpty())
            return true;

    return false;
}

void StartPageActionsModel::reset()
{
    removeRows(0, rowCount());

    emit layoutChanged();
}
