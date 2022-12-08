#pragma once

#include <QSortFilterProxyModel>

/**
 * Loaded plugins filter model class
 *
 * Sorting and filtering model for loaded plugins
 *
 * @author Thomas Kroes
 */
class LoadedPluginsFilterModel : public QSortFilterProxyModel
{
public:

    /** 
     * Construct from \p parent
     * @param parent Pointer to parent object
    */
    LoadedPluginsFilterModel(QObject* parent = nullptr);

    /**
     * Returns whether \p row with \p parent is filtered out (false) or in (true)
     * @param row Row index
     * @param parent Parent index
     * @return Boolean indicating whether the item is filtered in or out
     */
    bool filterAcceptsRow(int row, const QModelIndex& parent) const override;

    /**
     * Compares two model indices plugin \p lhs with \p rhs
     * @param lhs Left-hand model index
     * @param rhs Right-hand model index
     */
    bool lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const override;
};
