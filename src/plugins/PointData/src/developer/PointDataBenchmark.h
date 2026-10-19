// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include <QMenu>

class PointDataBenchmark final
{
public:
    /** Adds the point-data benchmark action to a developer menu. */
    static void populateMenu(QMenu& menu, QWidget* owner);
};
