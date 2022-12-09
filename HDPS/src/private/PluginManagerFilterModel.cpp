#include "PluginManagerFilterModel.h"
#include "PluginManagerModel.h"

#include <QDebug>

#ifdef _DEBUG
    #define PLUGIN_MANAGER_FILTER_MODEL_VERBOSE
#endif

PluginManagerFilterModel::PluginManagerFilterModel(QObject* parent /*= nullptr*/) :
    QSortFilterProxyModel(parent)
{
}

bool PluginManagerFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    const auto index = sourceModel()->index(row, 0, parent);

    if (!index.isValid())
        return true;

    //const auto* messageRecord = static_cast<MessageRecord*>(index.internalPointer());

    if (filterRegularExpression().isValid()) {
        const auto key = sourceModel()->data(index.siblingAtColumn(filterKeyColumn()), filterRole()).toString();

        if (!key.contains(filterRegularExpression()))
            return false;
    }

    //const auto selectedFilterTypeOptions = _filterTypeAction.getSelectedOptions();

    //switch (messageRecord->type)
    //{
    //case QtMsgType::QtDebugMsg:
    //{
    //    if (!selectedFilterTypeOptions.contains("Debug"))
    //        return false;

    //    break;
    //}

    //case QtMsgType::QtWarningMsg:
    //{
    //    if (!selectedFilterTypeOptions.contains("Warning"))
    //        return false;

    //    break;
    //}

    //case QtMsgType::QtCriticalMsg:
    //{
    //    if (!selectedFilterTypeOptions.contains("Critical"))
    //        return false;

    //    break;
    //}

    //case QtMsgType::QtFatalMsg:
    //{
    //    if (!selectedFilterTypeOptions.contains("Fatal"))
    //        return false;

    //    break;
    //}

    //case QtMsgType::QtInfoMsg:
    //{
    //    if (!selectedFilterTypeOptions.contains("Info"))
    //        return false;

    //    break;
    //}

    //default:
    //    break;
    //}

    return true;
}

bool PluginManagerFilterModel::lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const
{
    return false;
}
