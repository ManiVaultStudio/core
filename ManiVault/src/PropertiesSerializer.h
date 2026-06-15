// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "workflow/WorkflowPlan.h"

#include <QObject>

namespace mv
{

/**
 * PropertiesSerializer is a utility class that provides static methods for serializing and deserializing (dataset) properties.
 *
 * It uses optimized QVariant serialization that is more efficient than the default QVariant serialization, and it includes a
 * format version in the serialized data to ensure compatibility between different versions of the software.
 *
 * The toVariantMap method takes a QVariantMap and returns a new QVariantMap that contains the serialized properties, while the
 * fromVariantMap method takes a source QVariantMap and a reference to a destination QVariantMap, and it populates the destination
 * map with the deserialized properties from the source map. If the format version in the source map is not supported, it throws a runtime error.
 *
 * @note This class is temporary; it will most likely be removed in the future when a more robust solution for properties serialization is implemented.
 *
 * @author T. Kroes
 */
class PropertiesSerializer : public QObject
{
public:

    /** Format version for the serialized properties */
    static constexpr quint32 FormatVersion = 2;

    /**
     * Creates a workflow plan that serializes the given properties map into a QVariantMap using the optimized serialization format.
     *
     * @param propertiesMap The properties map to serialize.
     * @return A unique pointer to the workflow plan that performs the serialization.
     */
    static workflow::UniqueWorkflowPlan toVariantMapWorkflow(const QVariantMap& propertiesMap);

    /**
     * Deserializes the given properties map into a workflow plan.
     *
     * @param propertiesMap The properties map to deserialize.
     * @param destinationPropertiesMap The map to populate with the deserialized properties.
     * @param parentContext The parent workflow execution context.
     * @return A unique pointer to the workflow plan that performs the deserialization.
     */
    static workflow::UniqueWorkflowPlan fromVariantMapWorkflow(const QVariantMap& propertiesMap, QVariantMap& destinationPropertiesMap, workflow::SharedWorkflowExecutionContext parentContext = nullptr);

private: // Utility methods for optimized QVariant serialization and deserialization

    /**
     * Deserializes a QVariantMap that was serialized using the optimized serialization format.
     * @param source The QVariantMap to deserialize, which is expected to be in the optimized serialization format.
     * @param context The workflow execution context to use for any necessary operations during deserialization.
     * @return A QVariantMap containing the deserialized data.
     */
    static QVariantMap loadOptimizedVariantMap(const QVariantMap& source, const workflow::SharedWorkflowExecutionContext& context);

    /**
     * Deserializes a QVariant that was serialized using the optimized serialization format.
     * @param source The QVariant to deserialize, which is expected to be in the optimized serialization format.
     * @param context The workflow execution context to use for any necessary operations during deserialization.
     * @return A QVariant containing the deserialized data.
     */
    static QVariant loadOptimizedVariant(const QVariant& source, const workflow::SharedWorkflowExecutionContext& context);

    /**
     * Serializes a QVariantMap using an optimized serialization format that is more efficient than the default QVariant serialization.
     * @param source The QVariantMap to serialize.
     * @return A QVariantMap containing the serialized data in the optimized format.
     */
    static QVariantMap saveOptimizedVariantMap(const QVariantMap& source);

    /**
     * Serializes a QVariant using an optimized serialization format that is more efficient than the default QVariant serialization.
     * @param source The QVariant to serialize.
     * @return A QVariant containing the serialized data in the optimized format.
     */
    static QVariant saveOptimizedVariant(const QVariant& source);

    /**
     * Serializes a QVariantList using an optimized serialization format that is more efficient than the default QVariant serialization.
     * @param list The QVariantList to serialize.
     * @return A QVariant containing the serialized data in the optimized format.
     */
    static QVariant saveOptimizedVariantList(const QVariantList& list);
};

}