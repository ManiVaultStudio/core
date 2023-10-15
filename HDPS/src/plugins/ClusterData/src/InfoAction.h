// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "Dataset.h"
#include "ClusterData.h"
#include "ClustersAction.h"

#include "actions/StringAction.h"
#include "event/EventListener.h"

namespace mv {
    class CoreInterface;
}

using namespace mv;
using namespace mv::gui;
using namespace mv::util;

/**
 * Info action class
 *
 * Action class for displaying basic clusters info
 *
 * @author Thomas Kroes
 */
class InfoAction : public GroupAction
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param clusters Reference to clusters dataset
     */
    InfoAction(QObject* parent, Clusters& clusters);

public: // Action getters

    StringAction& getNumberOfClustersAction() { return _numberOfClustersAction; }

protected:
    Dataset<Clusters>       _clusters;                  /** Clusters dataset smart pointer */
    StringAction            _numberOfClustersAction;    /** Number of points action */
    ClustersAction          _clustersAction;            /** Clusters action */
    mv::EventListener     _eventListener;             /** Listen to HDPS events */
};
