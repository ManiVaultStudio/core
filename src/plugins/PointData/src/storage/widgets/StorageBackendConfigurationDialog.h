// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "../actions/StorageBackendConversionAction.h"
#include "../../pointdata_export.h"

#include <QDialog>

#include <memory>

/**
 * @brief Hosts backend-provided controls for conversion or configuration.
 *
 * The dialog deliberately knows nothing about a particular backend. It hosts
 * the conversion action supplied by the target backend and returns its options
 * after acceptance. It takes ownership of that action for the dialog lifetime;
 * the caller decides whether the returned options create a representation or
 * reconfigure an existing one.
 *
 * @author T. Kroes
 */
class POINTDATA_EXPORT StorageBackendConfigurationDialog final : public QDialog
{
public:
    /** Describes why the backend options are being presented. */
    enum class Purpose
    {
        Configure,
        Convert,
        Import
    };

    /** Takes ownership of the backend action, preserving the legacy update/conversion mode. */
    explicit StorageBackendConfigurationDialog(std::unique_ptr<mv::gui::StorageBackendConversionAction> conversionAction, bool updateMode = false, QWidget* parent = nullptr);

    /** Takes ownership of the backend-provided action and hosts it in the dialog. */
    explicit StorageBackendConfigurationDialog(std::unique_ptr<mv::gui::StorageBackendConversionAction> conversionAction, Purpose purpose, QWidget* parent = nullptr);

    /** Returns the options currently held by the embedded action, or null if it has no action. */
    mv::pointdata::storage::StorageBackendConversionOptionsPtr getConversionOptions() const;

private:
    std::unique_ptr<mv::gui::StorageBackendConversionAction> _conversionAction;
};
