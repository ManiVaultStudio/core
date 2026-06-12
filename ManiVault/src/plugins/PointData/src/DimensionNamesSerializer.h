// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <workflow/WorkflowContextBase.h>
#include <workflow/WorkflowPlan.h>
#include <workflow/WorkflowExecutionContext.h>

#include <QVariant>

class Points;

/**
 * Serializer for dimension names of PointData.
 *
 * This is a temporary solution to serialize dimension names until a more general solution for
 * serializing dataset metadata is implemented.
 *
 * @author T. Kroes
 */
class DimensionNamesSerializer : public QObject
{
public:


    static constexpr quint32 FormatVersion = 2;

    /**
     * Deserializes dimension names from the provided QVariantMap and sets them to the provided Points.
     * @param points The Points object to set the deserialized dimension names to.
     * @param variantMap The QVariantMap containing the serialized dimension names.
     * @param parentContext The parent workflow execution context.
     * @return A UniqueWorkflowPlan representing the deserialization workflow.
     */
    static mv::workflow::UniqueWorkflowPlan fromVariantMapWorkflow(Points* points, const QVariantMap& variantMap, mv::workflow::SharedWorkflowExecutionContext parentContext = nullptr);

    /**
     * Serializes the dimension names of a Points object into a QVariantMap.
     * @param dimensionNames The list of dimension names to serialize.
     * @return A UniqueWorkflowPlan representing the serialization workflow.
     */
    static mv::workflow::UniqueWorkflowPlan toVariantMapWorkflow(const std::vector<QString>& dimensionNames);
};
