// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "pointdata_export.h"

#include <actions/HorizontalGroupAction.h>
#include <actions/OptionAction.h>
#include <actions/TriggerAction.h>

#include <QStringList>

/**
 * @brief Presents reusable storage backend selection and configuration controls.
 *
 * The action owns the backend choice and configure trigger, but does not own
 * backend settings or perform conversions. Consumers handle the emitted
 * requests, allowing the controls to serve both existing datasets and import
 * dialogs. The backend choice includes read and write capability text derived
 * from the selected backend type. Backend-specific settings are supplied by
 * StorageBackendConversionAction implementations.
 *
 * @author T. Kroes
 */
class POINTDATA_EXPORT StorageBackendConfiguratorAction : public mv::gui::HorizontalGroupAction
{
    Q_OBJECT

public:
    /** Creates controls for the supplied backend choices and default selection. */
    StorageBackendConfiguratorAction(QObject* parent, const QStringList& backendTypes, const QString& defaultBackendType, const QString& title = "Storage backend");

    /** Returns the backend choice control. */
    mv::gui::OptionAction& getBackendTypeAction();

    /** Returns the configure trigger control. */
    mv::gui::TriggerAction& getConfigureAction();

    /** Enables or disables the configure trigger, typically for the active backend. */
    void setConfigureEnabled(bool enabled);

signals:
    /** Emitted when the selected backend type changes. */
    void backendTypeChanged(const QString& backendType);

    /** Emitted when the user requests configuration; the consumer presents backend-specific controls. */
    void configureRequested();

private:
    mv::gui::OptionAction _backendTypeAction;
    mv::gui::TriggerAction _configureAction;
};
