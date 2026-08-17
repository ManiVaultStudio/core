// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

class QObject;
class QMenu;

namespace mv::detail
{

/**
 * @brief Runs hidden parallel workflow test scenarios.
 *
 * The phantom suite is intentionally private to ManiVault Studio. It exercises
 * the high-level parallel API, workflow progress reporting, result
 * notifications, background task progress, task notifications, and cancellation
 * paths without exposing a regular user-facing command.
 */
class ParallelPhantomTestSuite final
{
public:

    /** Starts the suite if no suite run is already active. */
    static void run(QObject* owner);

    /** Populates a menu with the available workflow test scenarios. */
    static void populateMenu(QMenu& menu, QObject* owner);

private:

    ParallelPhantomTestSuite() = delete;
};

}
