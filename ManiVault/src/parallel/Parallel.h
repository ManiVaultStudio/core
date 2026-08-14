// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include "ParallelExecutionChain.h"

#include "workflow/WorkflowOptions.h"
#include "workflow/WorkflowPlan.h"
#include "workflow/WorkflowResult.h"

#include <memory>
#include <optional>
#include <QString>
#include <type_traits>
#include <utility>
#include <vector>

namespace mv
{

class CORE_EXPORT Parallel
{
public:
    template<typename Function>
    static workflow::SharedWorkflowResult run(const QString& name, Function&& function, const workflow::WorkflowOptions& options = {})
    {
        return stages(name).run(name, std::forward<Function>(function)).execute(options);
    }

    template<typename Range, typename Function>
    static workflow::SharedWorkflowResult forEach(const QString& name, Range&& range, Function&& function, const workflow::WorkflowOptions& options = {})
    {
        return stages(name).forEach(name, std::forward<Range>(range), std::forward<Function>(function)).execute(options);
    }

    template<typename Range, typename Function>
    static auto map(const QString& name, Range&& range, Function&& function, const workflow::WorkflowOptions& options = {})
    {
        auto items          = parallel_detail::makeOwnedRange(std::forward<Range>(range));
        auto functionPtr    = std::make_shared<std::decay_t<Function>>(std::forward<Function>(function));

        using Item      = typename std::remove_reference_t<decltype(*items)>::value_type;
        using Result    = std::decay_t<std::invoke_result_t<std::decay_t<Function>&, Item&>>;

        static_assert(!std::is_void_v<Result>, "Parallel::map() functions must return a value");

        auto results    = std::make_shared<std::vector<std::optional<Result>>>(items->size());
        auto plan       = std::make_unique<workflow::WorkflowPlan>(name.isEmpty() ? QStringLiteral("Parallel map") : name);

        workflow::WorkflowPlan::Jobs jobs;

        jobs.reserve(items->size());

        for (std::size_t index = 0; index < items->size(); ++index) {
            jobs.emplace_back(QStringLiteral("%1 %2").arg(name).arg(index + 1), [items, functionPtr, results, index](const workflow::WorkflowPlan::Job&, const workflow::SharedWorkflowExecutionContext&) {
                (*results)[index].emplace((*functionPtr)((*items)[index]));
            });
        }

        plan->addParallelStage(name, std::move(jobs));

        executePlan(std::move(plan), options);

        std::vector<Result> values;

        values.reserve(results->size());

        for (auto& result : *results)
            values.push_back(std::move(result.value()));

        return values;
    }

    static ParallelExecutionChain stages(const QString& name = {});

    static workflow::SharedWorkflowResult runPhantomTest(const workflow::WorkflowOptions& options = {});

private:

    static workflow::SharedWorkflowResult executePlan(workflow::UniqueWorkflowPlan plan, const workflow::WorkflowOptions& options);
};

}
