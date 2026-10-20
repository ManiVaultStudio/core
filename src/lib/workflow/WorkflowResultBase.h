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
 * @brief Stores common workflow result state.
 *
 * WorkflowResultBase stores the workflow entity name, status, optional QVariant
 * output value, and execution options associated with a result.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class CORE_EXPORT WorkflowResultBase
{
public:

    /**
     * @brief Defines the lifecycle outcome of a workflow result.
     */
    enum class Status {
        Undefined,      /**< Result status has not been set. */
        Success,        /**< Execution completed successfully. */
        Failed,         /**< Execution failed. */
        Canceled        /**< Execution was canceled. */
    };

public:

    /**
     * @brief Constructs a workflow result base.
     * @param workflowName Name of the workflow entity that produced this result.
     */
    WorkflowResultBase(const QString& workflowName);

public:

    /**
     * @brief Returns the workflow entity name.
     * @return Name of the workflow entity that produced this result.
     */
    [[nodiscard]] QString getWorkflowName() const;

    /**
     * @brief Returns the result status.
     * @return Current result status.
     */
    [[nodiscard]] Status getStatus() const;

    /**
     * @brief Returns the icon representing the result status.
     * @return Status icon.
     */
    [[nodiscard]] QIcon getStatusIcon() const;

    /**
     * @brief Sets the status of the workflow result.
     * @param status Status to store.
     */
    void setStatus(Status status);

    /**
     * @brief Stores the workflow options used during execution.
     * @param options Workflow options to store.
     */
    void setOptions(const WorkflowOptions& options);

    /**
     * @brief Returns the workflow options used during execution.
     * @return Stored workflow options.
     */
    [[nodiscard]] const WorkflowOptions& getOptions() const;

public:

    /**
     * @brief Checks whether a value is available.
     *
     * Use this before reading the QVariant output when the producing workflow
     * may not have emitted a value.
     *
     * @return True if the workflow result contains a valid value.
     */
    [[nodiscard]] bool hasValue() const;

    /**
     * @brief Retrieves the produced value.
     *
     * The value can contain any type supported by QVariant.
     *
     * @return Value produced by workflow execution.
     */
    [[nodiscard]] const QVariant& getValue() const;

    /**
     * @brief Sets the produced value.
     *
     * @param value Value to store.
     */
    void setValue(const QVariant& value);

    /**
     * @brief Moves the produced value into the result.
     *
     * @param value Value to move into the result.
     */
    void setValue(QVariant&& value);

    /**
     * @brief Converts and stores the produced value.
     *
     * @tparam T Value type.
     * @param value Value to convert and store.
     */
    template<typename T>
    void setValue(T&& value) {
        _value = QVariant::fromValue(std::forward<T>(value));
    }

    /**
     * @brief Checks whether the value can convert to a type.
     *
     * @tparam T Target value type.
     * @return True if the value can be converted to T.
     */
    template<typename T>
    [[nodiscard]] bool canConvert() const {
        return _value.canConvert<T>();
    }

    /**
     * @brief Retrieves the value as a type.
     *
     * Check canConvert<T>() first when conversion is uncertain.
     *
     * @tparam T Target value type.
     * @return Value converted to T.
     */
    template<typename T>
    [[nodiscard]] T value() const {
        return _value.value<T>();
    }

    /**
     * @brief Tries to retrieve the value as a type.
     *
     * @tparam T Target value type.
     * @return Converted value, or std::nullopt when conversion is not possible.
     */
    template<typename T>
    [[nodiscard]] std::optional<T> tryValue() const
    {
        if (!_value.isValid() || !_value.canConvert<T>())
            return std::nullopt;

        return _value.value<T>();
    }

private:

    QString             _workflowName;                  /**< Name of the workflow entity that produced this result. */
    Status              _status = Status::Undefined;    /**< Result status. */
    QVariant            _value;                         /**< Optional value produced by execution. */
    WorkflowOptions     _options;                       /**< Workflow options used for execution. */
};

}
