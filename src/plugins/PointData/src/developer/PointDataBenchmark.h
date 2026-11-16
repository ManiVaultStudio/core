// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include <QMenu>

/**
 * @brief Adds the point-data discovery and benchmark tool to a developer menu.
 *
 * The tool benchmarks selected concrete point datasets already present in the
 * current data hierarchy. It does not create or materialize fixture data; the
 * loaded datasets retain ownership of their storage backends and cache state.
 * Benchmark results are written to the selected CSV file, and the workflow
 * can be canceled through its modal progress task.
 *
 * @author Thomas Kroes
 */
class PointDataBenchmark final
{
public:
    /** Adds the point-data benchmark action to a developer menu. */
    static void populateMenu(QMenu& menu, QWidget* owner);
};
