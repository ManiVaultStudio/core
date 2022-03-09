#pragma once

#include <QSortFilterProxyModel>

namespace hdps {

namespace util {

/**
 * Presets filter model class
 *
 * @author Thomas Kroes
 */
class PresetsFilterModel : public QSortFilterProxyModel
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
    */
    PresetsFilterModel(QObject* parent);

    /**
     * Returns whether a give row with give parent is filtered out (false) or in (true)
     * @param row Row index
     * @param parent Parent index
     */
    bool filterAcceptsRow(int row, const QModelIndex& parent) const override;
};

}
}
