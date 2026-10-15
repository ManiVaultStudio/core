// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "pointdata_export.h"

#include <Dataset.h>
#include <actions/HorizontalGroupAction.h>
#include <actions/OptionAction.h>
#include <actions/TriggerAction.h>

class Points;

/**
 * @brief Displays the storage backend used by a points dataset.
 *
 * StorageBackendAction keeps the backend selection visible alongside the
 * point-data controls. The selector is intentionally read-only while tiled
 * storage remains an experimental read-only representation. The adjacent
 * configuration trigger reserves the standard entry point for future backend
 * configuration without exposing unsupported mutation behavior today.
 *
 * @author Thomas Kroes
 */
class POINTDATA_EXPORT StorageBackendAction final : public mv::gui::HorizontalGroupAction
{
    Q_OBJECT

public:
    /** Constructs a storage backend action for p points. */
    StorageBackendAction(QObject* parent, const mv::Dataset<Points>& points);

    /** Returns the backend selector action. */
    mv::gui::OptionAction& getBackendTypeAction();

    /** Returns the backend configuration trigger action. */
    mv::gui::TriggerAction& getConfigureAction();

private:
    void updateBackendType();

    mv::Dataset<Points>          _points;             /**< Points dataset whose backend is displayed. */
    mv::gui::OptionAction       _backendTypeAction;  /**< Read-only storage backend selector. */
    mv::gui::TriggerAction      _configureAction;    /**< Trigger reserved for backend configuration. */
};
