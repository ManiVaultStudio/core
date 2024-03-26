// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <DataHierarchyItem.h>

#include <actions/GroupsAction.h>

#include <QWidget>

using namespace mv;
using namespace mv::util;
using namespace mv::gui;

class DataPropertiesPlugin;

/**
 * Data properties widget class
 *
 * Widget class for viewing/editing dataset property pages
 *
 * @author Thomas Kroes
 */
class DataPropertiesWidget : public QWidget
{
public:

    /**
     * Construct with pointer to owning \p dataPropertiesPlugin and \p parent widget
     * @param dataPropertiesPlugin Pointer to owning data properties plugin
     * @param parent Pointer to parent widget
     */
    DataPropertiesWidget(DataPropertiesPlugin* dataPropertiesPlugin, QWidget* parent = nullptr);

protected:

    /** Callback when is called when the selected items in the data hierarchy changed */
    void dataHierarchySelectionChanged();

private:

    void beginPopulate();
    void populate();
    void endPopulate();

protected:
    DataPropertiesPlugin*   _dataPropertiesPlugin;      /** Pointer to owning data properties plugin */
    QVBoxLayout             _layout;                    /** Main layout */
    QTimer                  _populateTimer;             /** Timer for selectively populating the UI */
    DataHierarchyItems      _scheduledItems;            /** Items scheduled to be shown */
    DataHierarchyItems      _currentItems;              /** Items currently shown */
    bool                    _abortPopulate;             /** Boolean determining whether the UI build process should be terminated */
    bool                    _isPopulating;              /** Boolean determining whether the UI build process should be terminated */
    GroupsAction            _groupsAction;              /** Groups action */
    GroupsAction::Widget*   _groupsActionWidget;        /** Pointer to groups action widget (used to change label sizing) */
};
