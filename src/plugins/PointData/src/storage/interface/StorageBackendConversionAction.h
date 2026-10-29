// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "../interface/PointsStorageBackendTypes.h"

#include <actions/GroupAction.h>
#include <actions/OptionAction.h>
#include <PointData/pointdata_export.h>

namespace mv::gui
{

/**
 * @brief Describes the user controls and options for one backend conversion.
 *
 * A conversion action belongs to the backend that will receive the converted
 * data. This keeps backend-specific settings, validation, and presentation out
 * of the generic conversion dialog and allows future backends to expose their
 * own options without expanding StorageBackendAction for every new format.
 * The action does not perform conversion itself; it only owns the controls
 * from which the conversion options are collected.
 *
 * @author T. Kroes
 */
class POINTDATA_EXPORT StorageBackendConversionAction : public GroupAction
{
    Q_OBJECT

public:
    /** Constructs backend settings, optionally adding an element-type selector initialized to the current type. */
    StorageBackendConversionAction(QObject* parent, const QString& title, std::optional<std::uint32_t> currentElementTypeIndex = {});

    ~StorageBackendConversionAction() override = default;

    /** Returns the target element type selected for the conversion, when available. */
    std::optional<std::uint32_t> getTargetElementTypeIndex() const;

    /** Returns the target element size selected for the conversion, when available. */
    std::optional<std::size_t> getTargetElementSize() const;

    /** Returns the backend configured by this action. */
    virtual pointdata::storage::StorageBackendType getTargetBackendType() const = 0;

    /** Returns the current backend-specific conversion options. */
    virtual pointdata::storage::StorageBackendConfigurationOptionsPtr getConversionOptions() const = 0;

    /** Returns whether the current settings pass the action's known preflight checks. */
    virtual bool canApply() const { return true; }

signals:
    void optionsChanged();

protected:
    void notifyOptionsChanged() { emit optionsChanged(); }

private:
    OptionAction                  _elementTypeAction;     /**< Optional selector for the converted matrix element type. */
    bool                          _hasElementTypeAction = false; /**< Whether an element-type selector is available. */
};

}
