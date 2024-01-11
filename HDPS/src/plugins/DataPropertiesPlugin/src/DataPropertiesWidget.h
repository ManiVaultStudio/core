// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <DataHierarchyItem.h>
#include <Dataset.h>

#include <actions/GroupsAction.h>

#include <QWidget>

using namespace mv;
using namespace mv::util;
using namespace mv::gui;

/**
 * Data properties widget class
 *
 * Widget class for viewing/editing dataset property pages
 *
 * @author Thomas Kroes
 */
class DataPropertiesWidget : public QWidget
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     */
    DataPropertiesWidget(QWidget* parent);

protected:

    /** Callback when is called when the selected items in the data hierarchy changed */
    void dataHierarchySelectionChanged();

protected:
    Dataset<DatasetImpl>    _dataset;               /** Smart point to current dataset */
    QVBoxLayout             _layout;                /** Main layout */
    GroupsAction            _groupsAction;          /** Groups action */
    GroupsAction::Widget*   _groupsActionWidget;    /** Pointer to groups action widget (used to change label sizing) */
};
