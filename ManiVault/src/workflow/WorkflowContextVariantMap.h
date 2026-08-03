// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowContextBase.h"

#include <QMutex>
#include <QVariant>
#include <QVariantMap>

#include <memory>

namespace mv::workflow
{

/**
 * @brief Shares QVariantMap data between workflow stages.
 *
 * This context provides a thread-safe key/value store that can be used by
 * workflow stages to exchange intermediate data during (serialization or
 * deserialization) workflows.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class CORE_EXPORT WorkflowContextVariantMap final : public WorkflowContextBase
{
public:

    /**
     * @brief Returns the complete map.
     * @return Copy of the stored QVariantMap.
     */
    [[nodiscard]] QVariantMap getMap() const;

    /**
     * @brief Replaces the complete map with the provided one.
     * @param map Map to store.
     */
    void setMap(const QVariantMap& map);

    /**
     * @brief Checks whether a value exists for the specified key.
     * @param key Key to test for existence.
     * @return True if the key exists, otherwise false.
     */
    [[nodiscard]] bool contains(const QString& key) const;

    /**
     * @brief Returns a value from the map.
     * @param key Key to retrieve.
     * @param defaultValue Value returned when the key is not present.
     * @return Stored value, or the provided default value.
     */
    [[nodiscard]] QVariant value(const QString& key, const QVariant& defaultValue = {}) const;

    /**
     * @brief Returns a QVariantMap value from the map.
     * @param key Key to retrieve.
     * @return The stored value converted to QVariantMap, or an empty map if the
     * key does not exist or cannot be converted.
     */
    [[nodiscard]] QVariantMap mapValue(const QString& key) const;

    /**
     * @brief Stores a value in the map.
     * @param key Key to store.
     * @param value Value to associate with the key.
     */
    void setValue(const QString& key, const QVariant& value);

    /**
     * @brief Removes and returns a value from the map.
     * @param key Key to remove.
     * @return The removed value or an invalid QVariant if the key was not found.
     */
    QVariant takeValue(const QString& key);

    /**
     * @brief Merges key/value pairs into the map.
     * @param values Map containing values to merge.
     */
    void merge(const QVariantMap& values);

    /**
     * @brief Clears all stored values.
     */
    void clear();

private:

    mutable QMutex  _mutex;     /**< Synchronizes access to the map. */
    QVariantMap     _map;       /**< Shared workflow data. */
};

/** Unique ownership pointer type for QVariantMap workflow contexts. */
using UniqueVariantMapWorkflowContext = std::unique_ptr<WorkflowContextVariantMap>;

/** Shared ownership pointer type for QVariantMap workflow contexts. */
using SharedVariantMapWorkflowContext = std::shared_ptr<WorkflowContextVariantMap>;

}


