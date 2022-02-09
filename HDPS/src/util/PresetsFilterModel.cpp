#include "PresetsFilterModel.h"
#include "PresetsModel.h"

namespace hdps {

namespace util {

PresetsFilterModel::PresetsFilterModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{
}

bool PresetsFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    return true;
}

}
}
