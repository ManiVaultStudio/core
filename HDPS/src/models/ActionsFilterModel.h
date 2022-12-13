#pragma once

#include "actions/StringAction.h"
#include "actions/OptionsAction.h"

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

    /** Construct with parent \p parent object
     * @param parent Pointer to parent object
    */
    ActionsFilterModel(QObject* parent = nullptr);

    /**
     * Returns whether \p row with \p parent is filtered out (false) or in (true)
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

public: // Action getters

    gui::StringAction& getTypeFilterAction() { return _typeFilterAction; }
    gui::OptionsAction& getScopeFilterAction() { return _scopeFilterAction; }

private:
    gui::StringAction   _typeFilterAction;      /** Action for filtering by action type */
    gui::OptionsAction  _scopeFilterAction;     /** Action for filtering based on action scope */
};

}
