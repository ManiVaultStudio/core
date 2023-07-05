// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "clusterdata_export.h"

#include <actions/WidgetActionWidget.h>

#include "ClustersModel.h"
#include "ClustersFilterModel.h"
#include "RemoveClustersAction.h"
#include "MergeClustersAction.h"
#include "FilterClustersAction.h"
#include "SelectClustersAction.h"
#include "SubsetAction.h"
#include "RefreshClustersAction.h"

#include <QItemSelectionModel>
#include <QTreeView>

using namespace hdps::gui;
using namespace hdps::util;

class ClustersAction;

/**
 * Clusters action widget class
 *
 * Widget class for clusters action
 *
 * @author Thomas Kroes
 */
class ClustersActionWidget : public WidgetActionWidget {
public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     * @param clustersAction Pointer to clusters action
     * @param widgetFlags Widget flags for the configuration of the widget
     */
    ClustersActionWidget(QWidget* parent, ClustersAction* clustersAction, const std::int32_t& widgetFlags);

    /**
     * Get clusters action
     * @return Reference to clusters action
     */
    ClustersAction& getClustersAction();

    /**
     * Get filter model
     * @return Reference to filter model
     */
    ClustersFilterModel& getFilterModel();

    /**
     * Get selection model
     * @return Reference to selection model
     */
    QItemSelectionModel& getSelectionModel();

protected:

    /** Sets up the selection synchronization (select points when clusters are selected) */
    void setupSelectionSynchronization();

protected:
    ClustersAction&             _clustersAction;            /** Reference to clusters action */
    ClustersFilterModel         _filterModel;               /** Clusters filter model */
    QItemSelectionModel         _selectionModel;            /** Clusters selection model */
    RemoveClustersAction        _removeClustersAction;      /** Remove clusters action */
    MergeClustersAction         _mergeClustersAction;       /** Merge clusters action */
    FilterClustersAction        _filterClustersAction;      /** Filter clusters action */
    SelectClustersAction        _selectClustersAction;      /** Select clusters action */
    SubsetAction                _subsetAction;              /** Subset action */
    RefreshClustersAction       _refreshClustersAction;     /** Refresh clusters action */
    QTreeView                   _clustersTreeView;          /** Clusters tree view */
};
