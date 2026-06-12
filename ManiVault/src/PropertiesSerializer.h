// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

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
     * Serializes the given properties map into a workflow plan.
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
};

}