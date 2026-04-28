// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include <QString>

namespace mv::util
{

class WorkflowExecutionContext;

class CORE_EXPORT WorkflowResult
{
public:
    WorkflowResult();

    explicit WorkflowResult(WorkflowExecutionContext* context);

    bool isValid() const;

    bool hasErrors() const;

    int getErrorCount() const;

    int getWarningCount() const;

    double getProgress() const;

    WorkflowExecutionContext* getContext() const;

    std::uint64_t getDuration() const;

    void setDuration(std::uint64_t duration);

    QString getErrorMessage() const;

private:
    WorkflowExecutionContext* _context;
    std::uint64_t   _duration;
    bool            _success = false;
};

using UniqueWorkflowResult = std::unique_ptr<WorkflowResult>;

} // namespace mv::util