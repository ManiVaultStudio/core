// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <util/AbstractWorkflow.h>
#include <util/WorkflowPlan.h>

#include "Archiver.h"

using namespace QtTaskTree;

class SerializePlanContext : public WorkflowContextBase
{
public:
};

using SerializePlanContextStorage = QtTaskTree::Storage<SerializePlanContext>;


class SerializePlanWorkflow final : public mv::util::AbstractWorkflow
{
public:
    explicit SerializePlanWorkflow(mv::util::SerializationPlan serializationPlan, mv::util::SharedOperationContext operationContext = {});

protected:

    QtTaskTree::Group makeRecipe() override;

    /**
     * Derived classes can override this method to perform any necessary initialization of the workflow context before the recipe is created. This allows for setting up any necessary data in the context that needs to be accessed by the tasks in the recipe, for example by initializing the workflow context with any necessary information for the workflow that needs to be accessed by the tasks in the recipe. The initializeContext method will be called with a reference to the workflow runtime context storage, allowing derived classes to implement custom logic for initializing the workflow context before the recipe is created.
     * @param context Reference to the workflow runtime context storage, used for passing data between tasks. The context will be initialized before the recipe is created and finalized after the recipe is done. The initializeContext method can be used for performing any necessary initialization of the workflow context before the recipe is created, for example by setting up any necessary data in the context that needs to be accessed by the tasks in the recipe. The initializeContext method will be called with a reference to the workflow runtime context storage, allowing derived classes to implement custom logic for initializing the workflow context before the recipe is created.
     */
    void setupStorage(WorkflowRuntimeContext& context) override;

    /**
     * Derived classes can override this method to perform any necessary handling when the workflow runtime context storage is done, for example by performing any necessary cleanup or by emitting a signal indicating that the storage is done. The onStorageDone method will be called with a reference to the workflow runtime context storage when the storage is done, allowing derived classes to perform any necessary handling when the workflow runtime context storage is done.
     * @param context Reference to the workflow runtime context storage, used for passing data between tasks. The onStorageDone method will be called with a reference to the workflow runtime context storage when the storage is done, allowing derived classes to perform any necessary handling when the workflow runtime context storage is done.
     */
    void onStorageDone(const WorkflowRuntimeContext& context) override;

    /**
     * Perform any necessary handling when the workflow is done, for example by performing any necessary cleanup or by emitting a signal indicating that the workflow is done. The handleDone method will be called with a DoneWith status when the workflow is done, allowing derived classes to perform any necessary handling when the workflow is done based on the completion status of the workflow.
     * @param status DoneWith status indicating whether the workflow finished successfully, with an error, or was canceled. The handleDone method will be called with a DoneWith status when the workflow is done, allowing derived classes to perform any necessary handling when the workflow is done based on the completion status of the workflow.
     */
    void handleDone(QtTaskTree::DoneWith status) override;

protected: // Workflow result initialization

    /**
     * Perform any necessary initialization of the workflow result before the workflow is executed. This allows for setting up any necessary information in the result that needs to be accessed after the workflow is done, for example by initializing the workflow result with any necessary information about the result of the workflow that needs to be accessed after the workflow is done. The initResult method will be called with a reference to the workflow result, allowing derived classes to implement custom logic for initializing the workflow result before the workflow is executed.
     * @param result Reference to the workflow result, used for storing the success flag and error message after the workflow is done. The initResult method can be used for performing any necessary initialization of the workflow result before the workflow is executed, for example by setting up any necessary information in the result that needs to be accessed after the workflow is done. The initResult method will be called with a reference to the workflow result, allowing derived classes to implement custom logic for initializing the workflow result before the workflow is executed.
     */
    void initResult(UniqueWorkflowResultBase& result) override;

private:

    static UniqueWorkflowContext createContext(mv::util::SerializationPlan serializationPlan);

    Group buildStageGroup(mv::util::SerializationPlan::Stage& stage);

    GroupItem buildJobTask(mv::util::SerializationPlan::Job& job);

private: // Stages

    void setup(SerializePlanContext& context);
    void serialize(SerializePlanContext& context);
    void finalize(SerializePlanContext& context);

private:
    mv::util::SerializationPlan     _serializationPlan;
};
