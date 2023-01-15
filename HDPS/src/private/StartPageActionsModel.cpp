#include "StartPageActionsModel.h"

#ifdef _DEBUG
    //#define START_PAGE_ACTIONS_MODEL_VERBOSE
#endif

QMap<StartPageActionsModel::Column, QPair<QString, QString>> StartPageActionsModel::columns = QMap<StartPageActionsModel::Column, QPair<QString, QString>>({
    { StartPageActionsModel::Column::Icon, { "Icon", "Icon" }},
    { StartPageActionsModel::Column::Title, { "Title", "Title" }},
    { StartPageActionsModel::Column::Description, { "Description", "Description" }},
    { StartPageActionsModel::Column::Comments, { "Comments", "Comments" }},
    { StartPageActionsModel::Column::Callback, { "Callback", "Callback which is called when the action is clicked" }},
    { StartPageActionsModel::Column::SummaryDelegate, { "Summary", "Delegate item with title and subtitle" }},
    { StartPageActionsModel::Column::CommentsDelegate, { "Summary", "Delegate item with title" }}
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

void StartPageActionsModel::add(const QIcon& icon, const QString& title, const QString& description, const QString& comments, const QString& tooltip, const ClickedCB& clickedCallback)
{
#ifdef START_PAGE_ACTIONS_MODEL_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    QList<QStandardItem*> startPageActionRow = {
        new QStandardItem(icon, ""),
        new QStandardItem(title),
        new QStandardItem(description),
        new QStandardItem(comments),
        new QStandardItem(),
        new QStandardItem(),
        new QStandardItem(),
    };

    startPageActionRow[static_cast<int>(Column::SummaryDelegate)]->setToolTip(tooltip);
    startPageActionRow[static_cast<int>(Column::Callback)]->setData(QVariant::fromValue(clickedCallback));

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
