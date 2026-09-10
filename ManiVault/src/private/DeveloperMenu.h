// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include <QMenu>

/** Menu containing development and integration test tools. */
class DeveloperMenu : public QMenu
{
public:
    /** Constructs the developer menu with an optional parent widget. */
    DeveloperMenu(QWidget* parent = nullptr);

private:
    /** Runs the safe handled-exception reporting test. */
    void testHandledException();

    /** Runs the destructive fatal-crash reporting test after confirmation. */
    void testFatalCrash();
};
