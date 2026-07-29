// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include "ParallelExecutionChain.h"

#include "workflow/WorkflowOptions.h"

#include <QString>

namespace mv
{

class CORE_EXPORT Parallel
{
public:
    template<typename Function>
    static void run(const QString& name, Function&& function, const workflow::WorkflowOptions& options = {});

    template<typename Range, typename Function>
    static void forEach(const QString& name, Range&& range, Function&& function, const workflow::WorkflowOptions& options = {});

    template<typename Range, typename Function>
    static auto map(const QString& name, Range&& range, Function&& function, const workflow::WorkflowOptions& options = {});

    static ParallelExecutionChain stages(const QString& name = {});
};

}
