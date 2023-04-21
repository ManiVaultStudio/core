#pragma once

#include "actions/StringAction.h"
#include "actions/OptionsAction.h"
#include "actions/ToggleAction.h"

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
    gui::ToggleAction& getConnectedFilterAction() { return _connectedFilterAction; }
    gui::OptionsAction& getHideInternalUseAction() { return _filterInternalUseAction; }
    gui::OptionsAction& getFilterEnabledAction() { return _filterEnabledAction; }
    gui::OptionsAction& getFilterVisibilityAction() { return _filterVisibilityAction; }
    gui::OptionsAction& getFilterMayPublishAction() { return _filterMayPublishAction; }
    gui::OptionsAction& getFilterMayConnectAction() { return _filterMayConnectAction; }
    gui::OptionsAction& getFilterMayDisconnectAction() { return _filterMayDisconnectAction; }
    gui::TriggerAction& getRemoveFiltersAction() { return _removeFiltersAction; }

private:
    gui::StringAction   _typeFilterAction;              /** Action for filtering by action type */
    QCompleter          _typeCompleter;                 /** Completer for type filter */
    gui::OptionsAction  _scopeFilterAction;             /** Action for filtering based on action scope */
    gui::ToggleAction   _connectedFilterAction;         /** Action for filtering based on whether an action is connected or not */
    gui::OptionsAction  _filterInternalUseAction;       /** Filter internal use action */
    gui::OptionsAction  _filterEnabledAction;           /** Filter enabled action */
    gui::OptionsAction  _filterVisibilityAction;        /** Filter visibility action */
    gui::OptionsAction  _filterMayPublishAction;        /** Filter may publish action */
    gui::OptionsAction  _filterMayConnectAction;        /** Filter may connect action */
    gui::OptionsAction  _filterMayDisconnectAction;     /** Filter may disconnect action */
    gui::TriggerAction  _removeFiltersAction;           /** Remove filters action */
};

}
