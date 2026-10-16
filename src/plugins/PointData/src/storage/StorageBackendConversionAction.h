// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "PointsStorageBackendTypes.h"

#include <actions/GroupAction.h>
#include <pointdata_export.h>

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
    StorageBackendConversionAction(QObject* parent, const QString& title) :
        GroupAction(parent, title)
    {
    }

    ~StorageBackendConversionAction() override = default;

    /** Returns the backend configured by this action. */
    virtual pointdata::storage::StorageBackendType getTargetBackendType() const = 0;

    /** Returns the current backend-specific conversion options. */
    virtual pointdata::storage::StorageBackendConversionOptionsPtr getConversionOptions() const = 0;

signals:
    void optionsChanged();

protected:
    void notifyOptionsChanged() { emit optionsChanged(); }
};

}
