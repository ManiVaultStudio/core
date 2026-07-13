// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include "WorkflowOptions.h"

#include <QString>
#include <QVariant>
#include <QIcon>

namespace mv::workflow
{

/**
 * @brief Base result shared by workflows, stages, and jobs.
 *
 * WorkflowResultBase stores the workflow entity name, status, optional QVariant
 * output value, and execution options associated with a result.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class CORE_EXPORT WorkflowResultBase
{
public:

    /** Status of a workflow result. */
    enum class Status {
        Undefined,      /**< Result status has not been set */
        Success,        /**< Execution completed successfully */
        Failed,         /**< Execution failed */
        Canceled        /**< Execution was canceled */
    };

public:

    /**
     * @brief Constructs a workflow result base.
     *
     * The workflow name is used for identification and logging.
     *
     * @param workflowName The name of the workflow that produced this result. This can be used for identification and logging purposes, allowing you to associate the result with a specific workflow execution.
     */
    WorkflowResultBase(const QString& workflowName);

public: // General

    /**
     * @brief Retrieves the workflow name.
     *
     * @return The name of the workflow that produced this result. This can be used for identification and logging purposes, allowing you to associate the result with a specific workflow execution.
     */
    QString getWorkflowName() const;

    /**
     * @brief Retrieves the status of the workflow result.
     * @return The status of the workflow result, which can be Undefined, Success, Failed, or Canceled.
     */
    Status getStatus() const;

    /**
     * @brief Retrieves the status icon.
     *
     * @return The icon representing the status of the workflow result. This can be used for visual representation in user interfaces, allowing users to quickly identify the status of the workflow execution.
     */
    QIcon getStatusIcon() const;

    /**
     * @brief Sets the status of the workflow result.
     * @param status The status to be set for the workflow result, which can be Undefined, Success, Failed, or Canceled.
     */
    void setStatus(Status status);

    /**
     * @brief Sets the workflow options associated with this result.
     * @param options The workflow options to be set for this result.
     */
    void setOptions(const WorkflowOptions& options);

    /**
     * @brief Retrieves the workflow options associated with this result.
     * @return The workflow options associated with this result.
     */
    const WorkflowOptions& getOptions() const;

public: // Value

    /**
     * @brief Checks whether a value is available.
     *
     * Use this before reading the QVariant output when the producing workflow
     * may not have emitted a value.
     *
     * @return True if the workflow result contains a valid value, false otherwise.
     */
    bool hasValue() const;

    /**
     * @brief Retrieves the produced value.
     *
     * The value can contain any type supported by QVariant.
     *
     * @return The value produced by the workflow execution.
     */
    const QVariant& getValue() const;

    /**
     * @brief Sets the produced value.
     *
     * @param value The value to be set for the workflow result.
     */
    void setValue(const QVariant& value);

    /**
     * @brief Moves the produced value into the result.
     *
     * @param value The value to be set for the workflow result.
     */
    void setValue(QVariant&& value);

    /**
     * @brief Converts and stores the produced value.
     *
     * @tparam T The type of the value being set.
     * @param value The value to be set for the workflow result.
     */
    template<typename T>
    void setValue(T&& value) {
        _value = QVariant::fromValue(std::forward<T>(value));
    }

    /**
     * @brief Checks whether the value can convert to a type.   
     *
     * @tparam T The type to which the value is being checked for conversion.
     * @return True if the value can be converted to the specified type T, false otherwise.
     */
    template<typename T>
    bool canConvert() const {
        return _value.canConvert<T>();
    }

    /**
     * @brief Retrieves the value as a type.
     *
     * Check canConvert<T>() first when conversion is uncertain.
     *
     * @tparam T The type to which the value is being accessed.
     * @return The value produced by the workflow execution as a specified type T.
     */
    template<typename T>
    T value() const {
        return _value.value<T>();
    }

    /**
     * @brief Tries to retrieve the value as a type.
     *
     * @tparam T The type to which the value is being accessed.
     * @return An optional containing the value produced by the workflow execution as a specified type T if it can be converted, or an empty optional if it cannot be converted.
     */
    template<typename T>
    std::optional<T> tryValue() const
    {
        if (!_value.isValid() || !_value.canConvert<T>())
            return std::nullopt;

        return _value.value<T>();
    }

private:

    QString             _workflowName;                  /**< Name of the workflow entity that produced this result */
    Status              _status = Status::Undefined;    /**< Result status */
    QVariant            _value;                         /**< Optional value produced by execution */
    WorkflowOptions     _options;                       /**< Workflow options used for execution */
};

}
