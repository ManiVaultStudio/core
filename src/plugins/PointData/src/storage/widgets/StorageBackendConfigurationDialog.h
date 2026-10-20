// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "../actions/StorageBackendConversionAction.h"

#include <QDialog>

#include <memory>

/**
 * @brief Hosts backend-provided controls for a storage conversion.
 *
 * The dialog deliberately knows nothing about a particular backend. It hosts
 * the conversion action supplied by the target backend and returns its options
 * after acceptance. This allows new backends to provide their own settings
 * without adding another hardcoded dialog implementation.
 *
 * @author T. Kroes
 */
class StorageBackendConfigurationDialog final : public QDialog
{
public:
    /** Takes ownership of the backend-provided action and hosts it in the dialog. */
    explicit StorageBackendConfigurationDialog(std::unique_ptr<mv::gui::StorageBackendConversionAction> conversionAction, bool updateMode = false, QWidget* parent = nullptr);

    /** Returns the options currently held by the embedded conversion action. */
    mv::pointdata::storage::StorageBackendConversionOptionsPtr getConversionOptions() const;

private:
    std::unique_ptr<mv::gui::StorageBackendConversionAction> _conversionAction;
};
