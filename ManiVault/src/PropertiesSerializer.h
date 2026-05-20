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

    /** Serializes the given QVariantMap using optimized QVariant serialization and includes the format version in the resulting map. */
    static QVariantMap toVariantMap(const QVariantMap& map);

    /**
     * Deserializes the properties from the given source QVariantMap into the destination QVariantMap. It checks the format version and throws a runtime error if it is not supported.
     * @param propertiesMap The source QVariantMap containing the serialized properties, which should include the "PropertiesFormatVersion" key with the format version.
     * @param destinationPropertiesMap The reference to the QVariantMap that will be populated with the deserialized properties. This map will be cleared before populating it with the deserialized properties.
     */
    static void fromVariantMap(const QVariantMap& propertiesMap, QVariantMap& destinationPropertiesMap);
};

}