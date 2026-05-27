// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "Dataset.h"
#include "ClusterData.h"
#include "ClustersAction.h"

#include "actions/StringAction.h"
#include "event/EventListener.h"

/**
 * Info action class
 *
 * Action class for displaying basic clusters info
 *
 * @author Thomas Kroes
 */
class InfoAction : public mv::gui::GroupAction
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param clusters Reference to clusters dataset
     */
    InfoAction(QObject* parent, Clusters& clusters);

public: // Action getters

    mv::gui::StringAction& getNumberOfClustersAction() { return _numberOfClustersAction; }

protected:
    mv::Dataset<Clusters>   _clusters;                  /** Clusters dataset smart pointer */
    mv::gui::StringAction   _numberOfClustersAction;    /** Number of points action */
    ClustersAction          _clustersAction;            /** Clusters action */
    mv::EventListener       _eventListener;             /** Listen to ManiniVault events */
};
