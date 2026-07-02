// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include <QString>
#include <QVariant>

namespace mv::workflow
{

/**
 * @brief Stores common result state for a workflow execution scope.
 *
 * WorkflowResultBase contains the shared result data used by workflow, stage,
 * and job results: the producing workflow name, completion status, and optional
 * QVariant output value. Derived result types add scope-specific information
 * such as messages, metrics, or execution duration.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class CORE_EXPORT WorkflowResultBase
{
public:

    /** Result status for a workflow execution scope. */
    enum class Status {
        Undefined,  /**< Status has not been set yet. */
        Success,    /**< Execution completed successfully. */
        Failed,     /**< Execution failed with an error. */
        Canceled    /**< Execution was canceled before completion. */
    };

public:

    /**
     * @brief Constructs a workflow result base.
     * @param workflowName Name of the workflow that produced this result.
     */
    WorkflowResultBase(const QString& workflowName);

public: // General

    /**
     * @brief Returns the name of the workflow that produced this result.
     * @return Workflow name.
     */
    [[nodiscard]] QString getWorkflowName() const;

    /**
     * @brief Returns the result status.
     * @return Current workflow result status.
     */
    [[nodiscard]] Status getStatus() const;

    /**
     * @brief Sets the result status.
     * @param status New workflow result status.
     */
    void setStatus(Status status);

public: // Value

    /**
     * @brief Returns whether this result contains an output value.
     * @return True if the stored QVariant is valid.
     */
    [[nodiscard]] bool hasValue() const;

    /**
     * @brief Returns the output value produced by this execution scope.
     * @return Stored QVariant value.
     */
    [[nodiscard]] const QVariant& getValue() const;

    /**
     * @brief Stores an output value.
     * @param value Value to store in this result.
     */
    void setValue(const QVariant& value);

    /**
     * @brief Stores an output value using move semantics.
     * @param value Value to move into this result.
     */
    void setValue(QVariant&& value);

    /**
     * @brief Stores an output value by converting it to QVariant.
     * @tparam T Value type.
     * @param value Value to store in this result.
     */
    template<typename T>
    void setValue(T&& value) {
        _value = QVariant::fromValue(std::forward<T>(value));
    }

    /**
     * @brief Returns whether the stored value can be converted to T.
     * @tparam T Target value type.
     * @return True if the stored QVariant can be converted to T.
     */
    template<typename T>
    [[nodiscard]] bool canConvert() const {
        return _value.canConvert<T>();
    }

    /**
     * @brief Returns the stored value converted to T.
     * @tparam T Target value type.
     * @return Stored value converted to T.
     */
    template<typename T>
    [[nodiscard]] T value() const {
        return _value.value<T>();
    }

    /**
     * @brief Attempts to return the stored value converted to T.
     * @tparam T Target value type.
     * @return Converted value, or std::nullopt if the value is invalid or cannot be converted.
     */
    template<typename T>
    [[nodiscard]] std::optional<T> tryValue() const
    {
        if (!_value.isValid() || !_value.canConvert<T>())
            return std::nullopt;

        return _value.value<T>();
    }

private:
    QString     _workflowName;                  /**< Name of the workflow that produced this result */
    Status      _status = Status::Undefined;    /**< Current result status */
    QVariant    _value;                         /**< Optional output value produced by this execution scope */
};

}
