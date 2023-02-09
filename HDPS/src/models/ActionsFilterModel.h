#pragma once

#include "actions/StringAction.h"
#include "actions/OptionsAction.h"

#include <QSortFilterProxyModel>
#include <QCompleter>

namespace hdps
{

class ActionsFilterModel : public QSortFilterProxyModel
{
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

    /**
     * Get action for filter model \p index
     * @param rowIndex Row index to retrieve the action for
     * @return Pointer to action (nullptr if not found)
     */
    gui::WidgetAction* getAction(std::int32_t rowIndex);

public: // Action getters

    gui::StringAction& getTypeFilterAction() { return _typeFilterAction; }
    gui::OptionsAction& getScopeFilterAction() { return _scopeFilterAction; }

private:
    gui::StringAction   _typeFilterAction;      /** Action for filtering by action type */
    QCompleter          _typeCompleter;         /** Completer for type filter */
    gui::OptionsAction  _scopeFilterAction;     /** Action for filtering based on action scope */
};

}
