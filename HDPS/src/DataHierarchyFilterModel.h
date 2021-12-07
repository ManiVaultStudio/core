#pragma once

#include <QSortFilterProxyModel>

/**
 * Data hierarchy filter model class
 *
 * A class for filtering and sorting the data hierarchy model
 *
 * @author Thomas Kroes
 */
class DataHierarchyFilterModel : public QSortFilterProxyModel {
public:

    /** Constructor
     * @param parent Pointer to parent object
    */
    DataHierarchyFilterModel(QObject* parent = nullptr);

    /**
     * Returns whether a give row with give parent is filtered out (false) or in (true)
     * @param row Row index
     * @param parent Parent index
     */
    bool filterAcceptsRow(int row, const QModelIndex& parent) const override;
};
