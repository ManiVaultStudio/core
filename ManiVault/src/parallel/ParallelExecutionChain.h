// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include "workflow/WorkflowOptions.h"
#include "workflow/WorkflowPlan.h"
#include "workflow/WorkflowResult.h"

#include <functional>
#include <iterator>
#include <memory>
#include <QString>
#include <type_traits>
#include <utility>
#include <vector>

namespace mv
{

namespace parallel_detail
{

template<typename Range>
using RangeValue = std::decay_t<decltype(*std::begin(std::declval<Range&>()))>;

template<typename Range>
auto makeOwnedRange(Range&& range)
{
    using Value = RangeValue<std::remove_reference_t<Range>>;

    auto items = std::make_shared<std::vector<Value>>();

    if constexpr (std::is_lvalue_reference_v<Range&&>) {
        for (auto&& item : range)
            items->emplace_back(item);
    }
    else {
        for (auto&& item : range)
            items->emplace_back(std::move(item));
    }

    return items;
}

template<typename Function, typename Item>
void invokeForEach(Function& function, Item& item, std::size_t index, const workflow::SharedWorkflowExecutionContext& context)
{
    if constexpr (std::is_invocable_v<Function&, Item&, std::size_t, const workflow::SharedWorkflowExecutionContext&>) {
        function(item, index, context);
    }
    else if constexpr (std::is_invocable_v<Function&, Item&, const workflow::SharedWorkflowExecutionContext&>) {
        function(item, context);
    }
    else if constexpr (std::is_invocable_v<Function&, Item&, std::size_t>) {
        function(item, index);
    }
    else if constexpr (std::is_invocable_v<Function&, Item&>) {
        function(item);
    }
    else {
        static_assert(std::is_invocable_v<Function&, Item&>,
            "Parallel forEach function must be callable with item, "
            "item and index, item and workflow context, or item, index, and workflow context.");
    }
}

template<typename Items, typename Function>
workflow::WorkflowPlan::Jobs makeForEachJobs(const QString& stageName, const std::shared_ptr<Items>& items, const std::shared_ptr<Function>& function)
{
    workflow::WorkflowPlan::Jobs jobs;

    jobs.reserve(items->size());

    for (std::size_t index = 0; index < items->size(); ++index) {
        jobs.emplace_back(QStringLiteral("%1 %2").arg(stageName).arg(index + 1), [items, function, index](const workflow::WorkflowPlan::Job&, const workflow::SharedWorkflowExecutionContext& context) {
            invokeForEach(*function, (*items)[index], index, context);
        });
    }

    return jobs;
}

}

class CORE_EXPORT ParallelExecutionChain
{
public:

    explicit ParallelExecutionChain(QString name = {});

    ParallelExecutionChain(const ParallelExecutionChain&) = delete;
    ParallelExecutionChain& operator=(const ParallelExecutionChain&) = delete;

    ParallelExecutionChain(ParallelExecutionChain&&) noexcept = default;
    ParallelExecutionChain& operator=(ParallelExecutionChain&&) noexcept = default;

    ~ParallelExecutionChain() = default;

    template<typename Function>
    ParallelExecutionChain& run(const QString& name, Function&& function) &
    {
        addStage(name, [name, function = std::forward<Function>(function)](workflow::WorkflowPlan& plan) mutable {
            plan.addSequentialStage(name, std::move(function));
        });

        return *this;
    }

    template<typename Function>
    ParallelExecutionChain&& run(const QString& name, Function&& function) &&
    {
        run(name, std::forward<Function>(function));

        return std::move(*this);
    }

    template<typename Range, typename Function>
    ParallelExecutionChain& forEach(const QString& name, Range&& range, Function&& function) &
    {
        auto items      = parallel_detail::makeOwnedRange(std::forward<Range>(range));
        auto functionPtr = std::make_shared<std::decay_t<Function>>(std::forward<Function>(function));

        addStage(name, [name, items = std::move(items), functionPtr](workflow::WorkflowPlan& plan) mutable {
            plan.addParallelStage(name, parallel_detail::makeForEachJobs(name, items, functionPtr));
        });

        return *this;
    }

    template<typename Range, typename Function>
    ParallelExecutionChain&& forEach(const QString& name, Range&& range, Function&& function) &&
    {
        forEach(name, std::forward<Range>(range), std::forward<Function>(function));

        return std::move(*this);
    }

    ParallelExecutionChain& then(ParallelExecutionChain other) &;

    ParallelExecutionChain&& then(ParallelExecutionChain other) &&;

    workflow::SharedWorkflowResult execute(const workflow::WorkflowOptions& options = {}) &&;

private:

    using AppendFunction = std::function<void(workflow::WorkflowPlan&)>;

    struct Stage
    {
        QString         name;       /**< Human-readable stage name */
        AppendFunction  appendTo;   /**< Appends the stage to a workflow plan */
    };

    void addStage(QString name, AppendFunction appendTo);

private:

    QString             _name;                      /**< Human-readable workflow name */
    std::vector<Stage>  _stages;                    /**< Lazy stage appenders */
    bool                _executed = false;          /**< Whether the chain has already executed */
};

}
