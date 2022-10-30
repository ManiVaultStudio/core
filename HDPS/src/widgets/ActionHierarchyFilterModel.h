#pragma once

#include <QSortFilterProxyModel>

/**
 * Action hierarchy filter model class
 *
 * A class for filtering and sorting the action hierarchy model
 *
 * @author Thomas Kroes
 */
class ActionHierarchyFilterModel : public QSortFilterProxyModel {
public:

    /** Constructor
     * @param parent Pointer to parent object
    */
    ActionHierarchyFilterModel(QObject* parent = nullptr);

    /**
     * Returns whether a given row with give parent is filtered out (false) or in (true)
     * @param row Row index
     * @param parent Parent index
     */
    bool filterAcceptsRow(int row, const QModelIndex& parent) const override;
};
