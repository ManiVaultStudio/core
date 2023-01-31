#pragma once

#include "actions/ToggleAction.h"
#include "actions/OptionsAction.h"
#include "actions/TriggerAction.h"

#include <QSortFilterProxyModel>

using namespace hdps::gui;

/**
 * Action hierarchy filter model class
 *
 * A class for filtering and sorting the action hierarchy model
 *
 * @author Thomas Kroes
 */
class ActionHierarchyFilterModel : public QSortFilterProxyModel
{
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

public: // Action getters

    OptionsAction& getHideInternalUseAction() { return _filterInternalUseAction; }
    OptionsAction& getFilterEnabledAction() { return _filterEnabledAction; }
    OptionsAction& getFilterVisibilityAction() { return _filterVisibilityAction; }
    OptionsAction& getFilterMayPublishAction() { return _filterMayPublishAction; }
    OptionsAction& getFilterMayConnectAction() { return _filterMayConnectAction; }
    OptionsAction& getFilterMayDisconnectAction() { return _filterMayDisconnectAction; }
    TriggerAction& getRemoveFiltersAction() { return _removeFiltersAction; }

private:
    OptionsAction   _filterInternalUseAction;       /** Filter internal use action */
    OptionsAction   _filterEnabledAction;           /** Filter enabled action */
    OptionsAction   _filterVisibilityAction;        /** Filter visibility action */
    OptionsAction   _filterMayPublishAction;        /** Filter may publish action */
    OptionsAction   _filterMayConnectAction;        /** Filter may connect action */
    OptionsAction   _filterMayDisconnectAction;     /** Filter may disconnect action */
    TriggerAction   _removeFiltersAction;           /** Remove filters action */
};
