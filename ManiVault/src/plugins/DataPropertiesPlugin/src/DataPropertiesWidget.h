// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <DataHierarchyItem.h>

#include <actions/GroupsAction.h>

#include <QWidget>

class DataPropertiesPlugin;

/**
 * @brief Widget for displaying dataset property pages.
 *
 * DataPropertiesWidget observes the current data hierarchy selection and
 * rebuilds its grouped property-page UI for the selected data hierarchy items.
 * Population is scheduled through a timer so expensive UI rebuilding can be
 * coalesced when the selection changes rapidly.
 *
 * @maintainer Thomas Kroes
 */
class DataPropertiesWidget : public QWidget
{
public:

    /**
     * @brief Constructs a data properties widget.
     * @param dataPropertiesPlugin Owning data properties plugin.
     * @param parent Optional parent widget.
     */
    DataPropertiesWidget(DataPropertiesPlugin* dataPropertiesPlugin, QWidget* parent = nullptr);

protected:

    /** Handles changes to the selected data hierarchy items. */
    void dataHierarchySelectionChanged();

private:

    /** Begins a scheduled UI population pass. */
    void beginPopulate();

    /** Populates the property UI for scheduled items. */
    void populate();

    /** Finishes the current UI population pass. */
    void endPopulate();

protected:

    DataPropertiesPlugin*           _dataPropertiesPlugin;      /**< Owning data properties plugin */
    QVBoxLayout                     _layout;                    /**< Main layout */
    QTimer                          _populateTimer;             /**< Timer used to schedule UI population */
    mv::DataHierarchyItems          _scheduledItems;            /**< Items scheduled to be shown */
    mv::DataHierarchyItems          _currentItems;              /**< Items currently shown */
    bool                            _abortPopulate;             /**< Whether the current UI population pass should abort */
    bool                            _isPopulating;              /**< Whether the widget is currently populating its UI */
    mv::gui::GroupsAction           _groupsAction;              /**< Groups action containing property page actions */
    mv::gui::GroupsAction::Widget*  _groupsActionWidget;        /**< Groups action widget used to adjust label sizing */
};
