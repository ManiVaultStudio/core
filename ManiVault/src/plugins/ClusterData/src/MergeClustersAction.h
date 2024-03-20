// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <actions/TriggerAction.h>

using namespace mv;
using namespace mv::gui;

class ClustersActionWidget;

/**
 * Merge clusters action class
 *
 * Action class for merging selected clusters in a clusters model
 *
 * @author Thomas Kroes
 */
class MergeClustersAction : public TriggerAction
{
public:

    /**
     * Constructor
     * @param clustersActionWidget Pointer to clusters action widget
     */
    MergeClustersAction(ClustersActionWidget* clustersActionWidget);

protected:
    ClustersActionWidget*   _clustersActionWidget;      /** Pointer to clusters action widget */
};
