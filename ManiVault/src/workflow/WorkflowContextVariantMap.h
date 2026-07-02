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
 * @brief Workflow context for sharing QVariantMap-based data between workflow stages.
 *
 * This context provides a thread-safe key/value store that can be used by
 * workflow stages to exchange intermediate data during (serialization or
 * deserialization) workflows.
 *
 * Typical usage:
 *
 * @code
 * auto context = std::make_shared<VariantMapWorkflowContext>();
 *
 * context->setValue("Dataset", datasetMap);
 * context->setValue("Children", childrenMap);
 *
 * const auto datasetMap = context->value("Dataset").toMap();
 * @endcode
 */
class CORE_EXPORT WorkflowContextVariantMap final : public WorkflowContextBase
{
public:

    /**
     * @brief Gets the complete map.
     * @return A copy of the stored QVariantMap.
     */
    [[nodiscard]] QVariantMap getMap() const;

    /**
     * @brief Replaces the complete map with the provided one.
     * @param map The new map to store.
     */
    void setMap(const QVariantMap& map);

    /**
     * @brief Checks whether a value exists for the specified key.
     * @param key Key to test for existence.
     * @return True if the key exists, otherwise false.
     */
    bool contains(const QString& key) const;

    /**
     * @brief Gets a value from the map.
     * @param key Key to retrieve.
     * @param defaultValue Value returned when the key is not present.
     * @return The stored value or the provided default value.
     */
    [[nodiscard]] QVariant value(const QString& key, const QVariant& defaultValue = {}) const;

    /**
     * @brief Gets a QVariantMap value from the map.
     * @param key Key to retrieve.
     * @return The stored value converted to QVariantMap, or an empty map if the
     * key does not exist or cannot be converted.
     */
    [[nodiscard]] QVariantMap mapValue(const QString& key) const;

    /**
     * @brief Stores a value in the map.
     * @param key Key to store.
     * @param value Value to associate with the key.
     *
     * If the key already exists its value is replaced.
     */
    void setValue(const QString& key, const QVariant& value);

    /**
     * @brief Removes and returns a value from the map.
     * @param key Key to remove.
     * @return The removed value or an invalid QVariant if the key was not found.
     */
    [[nodiscard]] QVariant takeValue(const QString& key);

    /**
	 * @brief Merges all key/value pairs from another QVariantMap into the result map.
	 * @param values Map containing values to merge.
	 *
	 * Existing keys are overwritten by values from the supplied map.
	 */
    void merge(const QVariantMap& values);

    /**
     * @brief Clears all stored values.
     */
    void clear();

private:
    mutable QMutex  _mutex;     /** Synchronizes access to the map. */
    QVariantMap     _map;       /** Shared workflow data. */
};

using UniqueVariantMapWorkflowContext = std::unique_ptr<WorkflowContextVariantMap>;
using SharedVariantMapWorkflowContext = std::shared_ptr<WorkflowContextVariantMap>;

}


