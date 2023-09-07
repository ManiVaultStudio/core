// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <actions/TriggerAction.h>

using namespace hdps;
using namespace hdps::gui;

class ClustersActionWidget;

/**
 * Remove clusters action class
 *
 * Action class for removing selected clusters from a clusters model
 *
 * @author Thomas Kroes
 */
class RemoveClustersAction : public TriggerAction
{
public:

    /**
     * Constructor
     * @param clustersActionWidget Pointer to clusters action widget
     */
    RemoveClustersAction(ClustersActionWidget* clustersActionWidget);

protected:
    ClustersActionWidget*   _clustersActionWidget;      /** Pointer to clusters action widget */
};
