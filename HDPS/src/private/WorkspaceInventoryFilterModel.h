#pragma once

#include <actions/OptionsAction.h>

#include <QSortFilterProxyModel>

/**
 * Workspace inventory filter model class
 *
 * Sorting and filtering model for the workspace inventory model
 *
 * @author Thomas Kroes
 */
class WorkspaceInventoryFilterModel : public QSortFilterProxyModel
{
public:

    /** 
     * Construct the filter model with \p parent
     * @param parent Pointer to parent object
    */
    WorkspaceInventoryFilterModel(QObject* parent = nullptr);

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

public:
    hdps::gui::OptionsAction& getFilterTypesAction() { return _filterTypesAction; }

private:
    hdps::gui::OptionsAction    _filterTypesAction;      /** Workspace filter types action */
};
