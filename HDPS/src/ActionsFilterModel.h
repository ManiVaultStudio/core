#pragma once

#include <QSortFilterProxyModel>

namespace hdps
{

class ActionsFilterModel : public QSortFilterProxyModel
{
public:

    /** Action scope filter flags */
    enum ScopeFilter {
        None        = 0x00001,      /** Do no filter actions based on their scope */
        Private     = 0x00002,      /** Filter private actions */
        Public      = 0x00004       /** Filter public actions */
    };

public:

    /** Constructor
     * @param parent Pointer to parent object
    */
    ActionsFilterModel(QObject* parent = nullptr);

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

    /**
     * Get action name filter string
     * @return Action name filter string
     */
    QString getNameFilter() const;

    /**
     * Set action name filter string
     * @param nameFilter Action name filter string
     */
    void setNameFilter(const QString& nameFilter);

    /**
     * Get action type filter string
     * @return Action type filter string
     */
    QString getTypeFilter() const;

    /**
     * Set action type filter string
     * @param typeFilter Action type filter string
     */
    void setTypeFilter(const QString& typeFilter);

    /**
     * Get scope filter
     * @return Action scope filter
     */
    ScopeFilter getScopeFilter() const;

    /**
     * Set scope filter
     * @param scopeFilter Scope filter
     */
    void setScopeFilter(const ScopeFilter& scopeFilter);

protected:

    /**
     * Filter by action name
     * @param index Model index for the action name column
     * @return Whether the action name passes the filter
     */
    bool filterName(const QModelIndex& index) const;

    /**
     * Filter by action type
     * @param index Model index for the action type column
     * @return Whether the action type passes the filter
     */
    bool filterType(const QModelIndex& index) const;

    /**
     * Filter by action scope (private/public)
     * @param index Model index for the action scope column
     * @return Whether the action scope passes the filter
     */
    bool filterScope(const QModelIndex& index) const;

private:
    QString         _nameFilter;        /** Action name filter */
    QString         _typeFilter;        /** Action type filter */
    ScopeFilter     _scopeFilter;       /** Action scope filter */
};

}
