// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <util/AbstractSerializationPlanExecutor.h>
#include <util/SerializationPlan.h>

#include <QString>

class TaskTreeSerializationPlanExecutor final : public mv::util::AbstractSerializationPlanExecutor
{
public:

    void execute(mv::util::SerializationPlan& serializationPlan) override;

private:
};

