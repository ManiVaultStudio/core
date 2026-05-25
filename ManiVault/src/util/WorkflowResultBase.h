// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include <QString>
#include <QVariant>

namespace mv::util
{

/**
 * @brief The WorkflowResultBase class represents the base structure for storing the results of a workflow execution, or a part of it (stage or job).
 *
 * @author Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class CORE_EXPORT WorkflowResultBase
{
public:

    /** Represents the status of a workflow result. This can be used to indicate whether the workflow execution was successful, failed, or canceled. */
    enum class Status {
        Undefined,      /** The status of the workflow result is undefined. This can be used as a default value when the status has not been set or determined yet. */
        Success,        /** The workflow execution completed successfully. */
        Failed,         /** The workflow execution encountered an error and failed. */
        Canceled        /** The workflow execution was canceled before completion. */
    };

public:

    /**
     * @brief Constructs a WorkflowResultBase object with the specified workflow name. The workflow name is used for identification and logging purposes, allowing you to associate the result with a specific workflow execution.
     * @param workflowName The name of the workflow that produced this result. This can be used for identification and logging purposes, allowing you to associate the result with a specific workflow execution.
     */
    WorkflowResultBase(const QString& workflowName);

public: // General

    /**
     * @brief Retrieves the name of the workflow that produced this result. This can be used for identification and logging purposes, allowing you to associate the result with a specific workflow execution.
     * @return The name of the workflow that produced this result. This can be used for identification and logging purposes, allowing you to associate the result with a specific workflow execution.
     */
    QString getWorkflowName() const;

    /**
     * @brief Retrieves the status of the workflow result.
     * @return The status of the workflow result, which can be Undefined, Success, Failed, or Canceled.
     */
    Status getStatus() const;

    /**
     * @brief Sets the status of the workflow result.
     * @param status The status to be set for the workflow result, which can be Undefined, Success, Failed, or Canceled.
     */
    void setStatus(Status status);

public: // Value

    /**
     * @brief Checks if the workflow result contains a valid value. This can be used to determine whether the workflow execution produced a meaningful output that can be further processed or utilized. If this method returns false, it indicates that the workflow did not produce a valid result, and any attempt to access the value may result in an error or undefined behavior.
     * @return True if the workflow result contains a valid value, false otherwise.
     */
    bool hasValue() const;

    /**
     * @brief Retrieves the value produced by the workflow execution. This can be any type of data that can be stored in a QVariant, such as a result object, a data structure, or any other relevant output from the workflow. The caller can use this value to further process or utilize the results of the workflow execution. It is important to check if the value is valid using the hasValue() method before attempting to access it, to avoid potential errors or undefined behavior.
     * @return The value produced by the workflow execution.
     */
    const QVariant& getValue() const;

    /**
     * @brief Sets the value produced by the workflow execution. This can be any type of data that can be stored in a QVariant, such as a result object, a data structure, or any other relevant output from the workflow. The caller can use this method to store the results of the workflow execution, which can then be accessed by parent contexts or other parts of the workflow as needed. It is important to ensure that the value being set is valid and meaningful for the context of the workflow execution.
     * @param value The value to be set for the workflow result.
     */
    void setValue(const QVariant& value);

    /**
     * @brief Sets the value produced by the workflow execution using move semantics. This can be any type of data that can be stored in a QVariant, such as a result object, a data structure, or any other relevant output from the workflow. The caller can use this method to store the results of the workflow execution, which can then be accessed by parent contexts or other parts of the workflow as needed. It is important to ensure that the value being set is valid and meaningful for the context of the workflow execution.
     * @param value The value to be set for the workflow result.
     */
    void setValue(QVariant&& value);

    /**
     * @brief Sets the value produced by the workflow execution using perfect forwarding. This can be any type of data that can be stored in a QVariant, such as a result object, a data structure, or any other relevant output from the workflow. The caller can use this method to store the results of the workflow execution, which can then be accessed by parent contexts or other parts of the workflow as needed. It is important to ensure that the value being set is valid and meaningful for the context of the workflow execution.
     * @tparam T The type of the value being set.
     * @param value The value to be set for the workflow result.
     */
    template<typename T>
    void setValue(T&& value) {
        _value = QVariant::fromValue(std::forward<T>(value));
    }

    /**
     * @brief Checks if the value produced by the workflow execution can be converted to a specified type T. This can be used to determine if the value can be safely accessed and utilized as a specific type without causing errors or undefined behavior. If this method returns true, it indicates that the value can be converted to the specified type T, and the caller can proceed to access the value using the value<T>() method. If it returns false, it indicates that the value cannot be converted to the specified type T, and any attempt to access it as that type may result in an error or undefined behavior.
     * @tparam T The type to which the value is being checked for conversion.
     * @return True if the value can be converted to the specified type T, false otherwise.
     */
    template<typename T>
    bool canConvert() const {
        return _value.canConvert<T>();
    }

    /**
     * @brief Retrieves the value produced by the workflow execution as a specified type T. This can be used to access the value in a specific type that is relevant for further processing or utilization. It is important to check if the value can be converted to the specified type T using the canConvert<T>() method before attempting to access it, to avoid potential errors or undefined behavior. If the value cannot be converted to the specified type T, this method may throw an exception or result in an error.
     * @tparam T The type to which the value is being accessed.
     * @return The value produced by the workflow execution as a specified type T.
     */
    template<typename T>
    T value() const {
        return _value.value<T>();
    }

    /**
     * @brief Attempts to retrieve the value produced by the workflow execution as a specified type T. This can be used to safely access the value in a specific type that is relevant for further processing or utilization, while avoiding potential errors or undefined behavior. If the value can be converted to the specified type T, this method returns an optional containing the value as that type. If the value cannot be converted to the specified type T, this method returns an empty optional, indicating that the value cannot be accessed as that type.
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
    QString     _workflowName;                  /** The name of the workflow that produced this result. This can be used for identification and logging purposes. */
    Status      _status = Status::Undefined;    /** The status of the workflow execution, indicating whether it was successful, failed, or canceled. This can be used to determine the overall outcome of the workflow execution. */
    QVariant    _value;                         /** The value produced by the workflow execution. This can be any type of data that can be stored in a QVariant, such as a result object, a data structure, or any other relevant output from the workflow. */
};

} // namespace mv::util