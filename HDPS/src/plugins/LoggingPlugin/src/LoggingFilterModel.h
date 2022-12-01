#pragma once

#include <QSortFilterProxyModel>

class LoggingFilterModel : public QSortFilterProxyModel
{
public:

    /** Constructor
     * @param parent Pointer to parent object
    */
    LoggingFilterModel(QObject* parent = nullptr);

    /**
     * Returns whether a give row with give parent is filtered out (false) or in (true)
     * @param row Row index
     * @param parent Parent index
     * @return Boolean indicating whether the item is filtered in or out
     */
    bool filterAcceptsRow(int row, const QModelIndex& parent) const override;

    /**
     * Compares two cluster items
     * @param lhs Left cluster
     * @param rhs Right cluster
     */
    bool lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const override;
};