#include "StartPageActionsModel.h"

#ifdef _DEBUG
    #define START_PAGE_ACTIONS_MODEL_VERBOSE
#endif

QMap<StartPageActionsModel::Column, QPair<QString, QString>> StartPageActionsModel::columns = QMap<StartPageActionsModel::Column, QPair<QString, QString>>({
    { StartPageActionsModel::Column::Icon, { "Icon", "Action icon" }},
    { StartPageActionsModel::Column::Title, { "Title", "Action title" }},
    { StartPageActionsModel::Column::Description, { "Description", "Action description" }},
    { StartPageActionsModel::Column::Summary, { "Summary", "Summary delegate column" }}
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

void StartPageActionsModel::add(const QIcon& icon, const QString& title, const QString& description)
{
#ifdef START_PAGE_ACTIONS_MODEL_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    QList<QStandardItem*> workspaceRow = {
        new QStandardItem(icon, ""),
        new QStandardItem(title),
        new QStandardItem(description),
        new QStandardItem(),
    };

    for (auto item : workspaceRow)
        item->setEditable(false);

    appendRow(workspaceRow);

    emit layoutChanged();
}
