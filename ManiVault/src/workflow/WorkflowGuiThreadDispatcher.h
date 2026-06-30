// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include <QObject>

namespace mv::workflow
{

/**
 * @brief Dispatcher object that lives on the GUI thread.
 *
 * This QObject is used to marshal workflow operations to the GUI thread using
 * Qt's signal/slot mechanism or queued method invocations. It serves as a
 * dedicated receiver for requests originating from worker threads that require
 * execution on the application's main thread.
 *
 * The dispatcher itself contains no logic; it simply provides a QObject with
 * GUI thread affinity that can be used as a safe target for queued invocations.
 *
 * @note This object must be created on the GUI thread and must remain associated
 * with that thread for its entire lifetime.
 */
class WorkflowGuiThreadDispatcher : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Construct a GUI thread dispatcher.
     * @param parent Optional parent QObject.
     */
    explicit WorkflowGuiThreadDispatcher(QObject* parent = nullptr);
};

} // namespace mv::util