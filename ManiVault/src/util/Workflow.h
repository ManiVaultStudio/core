// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowResult.h"
#include "OperationContext.h"

#include <QtTaskTree>

namespace mv::util
{

class AbstractWorkflowPlanExecutor;
class WorkflowPlan;

/*
 * AbstractWorkflow is a base class for defining workflows in the application.
 * It provides a common interface for starting, canceling, and measuring the
 * duration of workflows, as well as for accessing the workflow runtime context
 * storage. Derived classes are expected to implement the actual workflow logic
 * by overriding the makeRecipe, initializeContext, setup, and finalize methods.
 *
 * @author T. Kroes
 */
class CORE_EXPORT Workflow
{
public:

    /*
     * Constructs an AbstractWorkflow with the given workflow context and title. The workflow context is a unique pointer to a WorkflowContextBase, which can be used for storing any necessary information for the workflow that needs to be accessed by the tasks in the workflow. The title is a descriptive name for the workflow that indicates what the workflow does, and it can be used for displaying the workflow in the UI or for logging purposes. The constructor initializes the workflow with the provided context and title, and it also initializes the duration to 0 and sets up the runner for executing the workflow.
     * @param workflowPlan A WorkflowPlan, which defines the sequence of tasks and their dependencies for the workflow. The workflow plan is used to create the recipe for the workflow, and it can be customized to define the specific tasks and their order of execution for the workflow.
     * @param title A descriptive name for the workflow that indicates what the workflow does, which can be used for displaying the workflow in the UI or for logging purposes. The title should be a descriptive name for the workflow that indicates what the workflow does, for example "Project Save Workflow" or "Data Import Workflow".
     * @param operationContext Optional shared pointer to an OperationContext, which can be used for reporting warnings and errors during the execution of the workflow. The OperationContext allows for better error handling and reporting during the execution of the workflow, and it can be shared across multiple workflows or tasks to provide a centralized way of reporting issues that occur during the execution of the workflow.
     */
    explicit Workflow(WorkflowPlan workflowPlan, SharedOperationContext operationContext = {});

    /**
     * Returns the title of the workflow, which can be used for displaying the workflow in the UI or for logging purposes. The title is set in the constructor and can be accessed using this method. The title should be a descriptive name for the workflow that indicates what the workflow does, for example "Project Save Workflow" or "Data Import Workflow".
     * @return The title of the workflow, which can be used for displaying the workflow in the UI or for logging purposes. The title is set in the constructor and can be accessed using this method. The title should be a descriptive name for the workflow that indicates what the workflow does, for example "Project Save Workflow" or "Data Import Workflow".
     */
    QString getTitle() const;

    /** Begins the execution of the workflow. This method starts the timer for measuring the duration of the workflow and performs any necessary setup before running the workflow. */
    void beginRun();

    /** Ends the execution of the workflow. This method stops the timer and records the duration of the workflow. */
    void endRun();

    /**
     * Cancels the workflow by calling the cancel method on the QtTaskTree runner. This will stop the execution of the workflow and any running tasks, and it will call the handleDone method with a DoneWith::Canceled status to indicate that the workflow was canceled. After canceling the workflow, the duration of the workflow can still be accessed using the getDuration method, which will return the duration from when the workflow was started until it was canceled.
      * @see handleDone
      * @see getDuration
     */
    void cancel();

    /**
     * Returns the result of the workflow, which contains the success status and any error message that occurred during the execution of the workflow. The workflow result is stored as a WorkflowResult object, which contains a boolean indicating whether the workflow was successful and a string for any error message that occurred during the execution of the workflow. The workflow result can be accessed after the workflow is done to check whether the workflow was successful and to retrieve any error message that occurred during the execution of the workflow.
     * @return The result of the workflow, which contains the success status and any error message that occurred during the execution of the workflow.
     */
    WorkflowResult getResult() const;

public: // Operation context

    /**
     * Access the operation context for the workflow, which can be used for storing any necessary information about the operation being performed by the workflow. This can be used for providing additional context to the tasks in the recipe, for example by allowing them to access information about the operation being performed by the workflow. The operation context is stored as a shared pointer, allowing it to be shared between the workflow and the tasks in the recipe, and it can be used for storing any necessary information about the operation being performed by the workflow that needs to be accessed by the tasks in the recipe.
     * @return A shared pointer to the operation context for the workflow, which can be used for storing any necessary information about the operation being performed by the workflow. This can be used for providing additional context to the tasks in the recipe, for example by allowing them to access information about the operation being performed by the workflow. The operation context is stored as a shared pointer, allowing it to be shared between the workflow and the tasks in the recipe, and it can be used for storing any necessary information about the operation being performed by the workflow that needs to be accessed by the tasks in the recipe.
     */
    const SharedOperationContext& getConstOperationContext() const;

    /**
     * Access the operation context for the workflow, which can be used for storing any necessary information about the operation being performed by the workflow. This can be used for providing additional context to the tasks in the recipe, for example by allowing them to access information about the operation being performed by the workflow. The operation context is stored as a shared pointer, allowing it to be shared between the workflow and the tasks in the recipe, and it can be used for storing any necessary information about the operation being performed by the workflow that needs to be accessed by the tasks in the recipe.
     * @return A shared pointer to the operation context for the workflow, which can be used for storing any necessary information about the operation being performed by the workflow. This can be used for providing additional context to the tasks in the recipe, for example by allowing them to access information about the operation being performed by the workflow. The operation context is stored as a shared pointer, allowing it to be shared between the workflow and the tasks in the recipe, and it can be used for storing any necessary information about the operation being performed by the workflow that needs to be accessed by the tasks in the recipe.
     */
    SharedOperationContext getOperationContext();

private:

    /**
     * Registers any necessary handlers for the workflow on the task tree. This method will be called in the start method after the recipe is created, allowing derived classes to register any necessary handlers for the workflow before the recipe is executed. The registerStorageHandlers method can be used for registering any necessary handlers for the workflow, for example by connecting signals from the workflow runtime context storage to custom slots in the derived class.
     * @param tree Reference to the task tree for the workflow, used for registering any necessary handlers for the workflow. This method will be called in the start method after the recipe is created, allowing derived classes to register any necessary handlers for the workflow before the recipe is executed. The registerStorageHandlers method can be used for registering any necessary handlers for the workflow, for example by connecting signals from the workflow runtime context storage to custom slots in the derived class.
     */
    //void registerStorageHandlers(QtTaskTree::QTaskTree& tree);

    SharedOperationContext  _operationContext;      /** Operation context for the workflow, used for storing any necessary information about the operation being performed by the workflow. This can be used for providing additional context to the tasks in the recipe, for example by allowing them to access information about the operation being performed by the workflow. The operation context is stored as a shared pointer, allowing it to be shared between the workflow and the tasks in the recipe, and it can be used for storing any necessary information about the operation being performed by the workflow that needs to be accessed by the tasks in the recipe. */
    WorkflowPlan            _workflowPlan;          /** Plan for the workflow, used for defining the sequence of tasks and their dependencies. */
    QElapsedTimer           _elapsedTimer;          /** Timer for measuring the duration of the workflow. The timer is started when the workflow is started, and it is used for measuring the duration of the workflow when it is done. The elapsed time can be accessed using the getDuration method, which will return the duration in milliseconds from when the workflow was started until it was done. */
    WorkflowResult          _result;                /** Result of the workflow, used for storing the success status and error message of the workflow. The workflow result is stored as a WorkflowResult object, which contains a boolean indicating whether the workflow was successful and a string for any error message that occurred during the execution of the workflow. The workflow result can be accessed after the workflow is done to check whether the workflow was successful and to retrieve any error message that occurred during the execution of the workflow. */
};

using UniqueWorkflow = std::unique_ptr<Workflow>;

} // namespace mv::util