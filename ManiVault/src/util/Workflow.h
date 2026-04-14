// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowRuntimeContext.h"
#include "WorkflowResultBase.h"
#include "OperationContext.h"

#include <QtTaskTree>

namespace mv::util
{

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

    using DoneCallback = std::function<void(bool success, const QString& error)>;

    struct WorkflowOutcome
    {
        bool success = true;
        QString error;
    };

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

    /**
     * Starts the workflow by creating the recipe using the makeRecipe method, initializing the context using the initializeContext method, and then running the recipe using the QtTaskTree runner. The workflow will execute the tasks defined in the recipe, and when it is done, it will call the handleDone method with the completion status of the workflow. The start method also starts a timer to measure the duration of the workflow, which can be accessed using the getDuration method after the workflow is done.
     * @see makeRecipe
     * @see initializeContext
     * @see handleDone
     * @see getDuration
     */
    void start();

    /**
     * Cancels the workflow by calling the cancel method on the QtTaskTree runner. This will stop the execution of the workflow and any running tasks, and it will call the handleDone method with a DoneWith::Canceled status to indicate that the workflow was canceled. After canceling the workflow, the duration of the workflow can still be accessed using the getDuration method, which will return the duration from when the workflow was started until it was canceled.
      * @see handleDone
      * @see getDuration
     */
    void cancel();

    /**
     * Returns the duration of the workflow in milliseconds. The duration is measured from the time the workflow is started until it is done, and it is stored after the workflow is done. This allows for measuring the duration of the workflow for performance monitoring or for displaying it to the user after the workflow is finished.
     * @return The duration of the workflow in milliseconds. The duration is measured from the time the workflow is started until it is done, and it is stored after the workflow is done. This allows for measuring the duration of the workflow for performance monitoring or for displaying it to the user after the workflow is finished.
     */
    quint64 getDuration() const;

    /**
     * Utility method for accessing the workflow context stored in the WorkflowRuntimeContextStorage. This method allows tasks in the recipe to access the workflow context and use the data stored in it for their execution, while also allowing for type safety when accessing the context. The contextAs method will attempt to dynamically cast the workflow context stored in the WorkflowRuntimeContextStorage to the specified type T, and it will return a pointer to the casted context if the cast is successful, or nullptr if the cast fails. This allows tasks in the recipe to access the workflow context and use the data stored in it for their execution, while also allowing for type safety when accessing the context.
     * @tparam T The type to which the workflow context should be cast. This should be a type that derives from WorkflowContextBase and is used for storing any necessary information for the workflow that needs to be accessed by the tasks in the workflow. The contextAs method will attempt to dynamically cast the workflow context stored in the WorkflowRuntimeContextStorage to the specified type T, and it will return a pointer to the casted context if the cast is successful, or nullptr if the cast fails. This allows tasks in the recipe to access the workflow context and use the data stored in it for their execution, while also allowing for type safety when accessing the context.
     * @param storage Reference to the workflow runtime context storage, used for passing data between tasks. The contextAs method will access the workflow context stored in the WorkflowRuntimeContextStorage and attempt to dynamically cast it to the specified type T, returning a pointer to the casted context if the cast is successful, or nullptr if the cast fails. This allows tasks in the recipe to access the workflow context and use the data stored in it for their execution, while also allowing for type safety when accessing the context.
     * @return A pointer to the casted workflow context if the cast is successful, or nullptr if the cast fails. This allows tasks in the recipe to access the workflow context and use the data stored in it for their execution, while also allowing for type safety when accessing the context.
     */
    //template<typename T>
    //static T* contextAs(WorkflowRuntimeContextStorage& storage)
    //{
    //    return dynamic_cast<T*>(storage->_workflowContext.get());
    //}

    /**
     * Const version of the contextAs method for accessing the workflow context stored in the WorkflowRuntimeContextStorage. This method allows tasks in the recipe to access the workflow context and use the data stored in it for their execution, while also allowing for type safety when accessing the context. The const version of the contextAs method will attempt to dynamically cast the workflow context stored in the WorkflowRuntimeContextStorage to the specified type T, and it will return a pointer to the casted context if the cast is successful, or nullptr if the cast fails. This allows tasks in the recipe to access the workflow context and use the data stored in it for their execution, while also allowing for type safety when accessing the context.
     * @tparam T The type to which the workflow context should be cast. This should be a type that derives from WorkflowContextBase and is used for storing any necessary information for the workflow that needs to be accessed by the tasks in the workflow. The const version of the contextAs method will attempt to dynamically cast the workflow context stored in the WorkflowRuntimeContextStorage to the specified type T, and it will return a pointer to the casted context if the cast is successful, or nullptr if the cast fails. This allows tasks in the recipe to access the workflow context and use the data stored in it for their execution, while also allowing for type safety when accessing the context.
     * @param storage Reference to the workflow runtime context storage, used for passing data between tasks. The const version of the contextAs method will access the workflow context stored in the WorkflowRuntimeContextStorage and attempt to dynamically cast it to the specified type T, returning a pointer to the casted context if the cast is successful, or nullptr if the cast fails. This allows tasks in the recipe to access the workflow context and use the data stored in it for their execution, while also allowing for type safety when accessing the context.
     * @return A pointer to the casted workflow context if the cast is successful, or nullptr if the cast fails. This allows tasks in the recipe to access the workflow context and use the data stored in it for their execution, while also allowing for type safety when accessing the context.s
     */
    //template<typename T>
    //static const T* contextAs(const WorkflowRuntimeContextStorage& storage)
    //{
    //    return dynamic_cast<const T*>(storage->_workflowContext.get());
    //}

    /**
     * Sets the callback function that will be called when the workflow is done, with a boolean indicating whether the workflow finished successfully and a QString containing any error message if the workflow finished with an error. This allows for notifying the caller of the workflow about the completion status of the workflow, and it can be used for performing any necessary handling based on whether the workflow finished successfully or with an error, for example by displaying a notification to the user or by logging the result of the workflow.
     * @param callback The callback function that will be called when the workflow is done, with a boolean indicating whether the workflow finished successfully and a QString containing any error message if the workflow finished with an error. This allows for notifying the caller of the workflow about the completion status of the workflow, and it can be used for performing any necessary handling based on whether the workflow finished successfully or with an error, for example by displaying a notification to the user or by logging the result of the workflow.
     */
    void setDoneCallback(DoneCallback callback);

protected:

    /*
     * Derived classes need to implement this method to define the recipe for the workflow, which is a tree of tasks that will be executed when the workflow is started. The recipe should be defined using the QtTaskTree API, which allows for defining complex workflows with dependencies between tasks, as well as for handling task setup and completion. The makeRecipe method will be called when the workflow is started, and the returned recipe will be executed by the workflow runner.
     * @return A QtTaskTree::Group that defines the recipe for the workflow, which is a tree of tasks that will be executed when the workflow is started. The recipe should be defined using the QtTaskTree API, which allows for defining complex workflows with dependencies between tasks, as well as for handling task setup and completion. The makeRecipe method will be called when the workflow is started, and the returned recipe will be executed by the workflow runner.
     */
    //virtual QtTaskTree::Group makeRecipe() = 0;

    /**
     * Derived classes can override this method to perform any necessary initialization of the workflow context before the recipe is created. This allows for setting up any necessary data in the context that needs to be accessed by the tasks in the recipe, for example by initializing the workflow context with any necessary information for the workflow that needs to be accessed by the tasks in the recipe. The initializeContext method will be called with a reference to the workflow runtime context storage, allowing derived classes to implement custom logic for initializing the workflow context before the recipe is created.
     * @param context Reference to the workflow runtime context storage, used for passing data between tasks. The context will be initialized before the recipe is created and finalized after the recipe is done. The initializeContext method can be used for performing any necessary initialization of the workflow context before the recipe is created, for example by setting up any necessary data in the context that needs to be accessed by the tasks in the recipe. The initializeContext method will be called with a reference to the workflow runtime context storage, allowing derived classes to implement custom logic for initializing the workflow context before the recipe is created.
     */
    //virtual void setupStorage(WorkflowRuntimeContext& context) = 0;

    /**
     * Derived classes can override this method to perform any necessary handling when the workflow runtime context storage is done, for example by performing any necessary cleanup or by emitting a signal indicating that the storage is done. The onStorageDone method will be called with a reference to the workflow runtime context storage when the storage is done, allowing derived classes to perform any necessary handling when the workflow runtime context storage is done.
     * @param context Reference to the workflow runtime context storage, used for passing data between tasks. The onStorageDone method will be called with a reference to the workflow runtime context storage when the storage is done, allowing derived classes to perform any necessary handling when the workflow runtime context storage is done.
     */
    //virtual void onStorageDone(const WorkflowRuntimeContext& context) = 0;

    /*
     * Derived classes can override this method to perform any necessary handling when the workflow is done, for example to measure the duration of the workflow or to emit a signal indicating that the workflow has finished. The handleDone method will be called with a DoneWith argument that indicates how the workflow was completed (successfully, with an error, or canceled), which can be used for performing any necessary handling based on the completion status of the workflow.
     * @param doneWith A DoneWith argument that indicates how the workflow was completed (successfully, with an error, or canceled), which can be used for performing any necessary handling based on the completion status of the workflow. The handleDone method will be called with this argument when the workflow is done, allowing derived classes to perform any necessary handling based on the completion status of the workflow.
     */
    virtual void handleDone(QtTaskTree::DoneWith doneWith);

protected: // Workflow result initialization and access

    /**
     * Perform any necessary initialization of the workflow result before the workflow is executed. This allows for setting up any necessary information in the result that needs to be accessed after the workflow is done, for example by initializing the workflow result with any necessary information about the result of the workflow that needs to be accessed after the workflow is done. The initResult method will be called with a reference to the workflow result, allowing derived classes to implement custom logic for initializing the workflow result before the workflow is executed.
     * @param result Reference to the workflow result, used for storing the success flag and error message after the workflow is done. The initResult method can be used for performing any necessary initialization of the workflow result before the workflow is executed, for example by setting up any necessary information in the result that needs to be accessed after the workflow is done. The initResult method will be called with a reference to the workflow result, allowing derived classes to implement custom logic for initializing the workflow result before the workflow is executed.
     */
    //virtual void initResult(UniqueWorkflowResultBase& result) = 0;

    /**
     * Utility method for accessing the workflow result stored in the _result member variable. This method allows for type-safe access to the workflow result, which is stored as a unique pointer to a WorkflowResultBase. The resultAs method will attempt to dynamically cast the workflow result stored in the _result member variable to the specified type ResultType, and it will return a pointer to the casted result if the cast is successful, or nullptr if the cast fails. This allows for type-safe access to the workflow result after the workflow is done, for example by allowing external code to access any necessary information about the result of the workflow that is stored in a derived class of Workflow
     * @tparam ResultType The type to which the workflow result should be cast. This should be a type that derives from WorkflowResultBase and is used for storing any necessary information about the result of the workflow that needs to be accessed after the workflow is done. The resultAs method will attempt to dynamically cast the workflow result stored in the _result member variable to the specified type ResultType, and it will return a pointer to the casted result if the cast is successful, or nullptr if the cast fails. This allows for type-safe access to the workflow result after the workflow is done, for example by allowing external code to access any necessary information about the result of the workflow that is stored in a derived class of Workflow
     * @return A pointer to the casted workflow result if the cast is successful, or nullptr if the cast fails. This allows for type-safe access to the workflow result after the workflow is done, for example by allowing external code to access any necessary information about the result of the workflow that is stored in a derived class of Workflow
     */
    //template<typename ResultType>
    //ResultType* resultAs()
    //{
    //    return dynamic_cast<ResultType*>(_result.get());
    //}

public: // Context storage access for tasks

    /** Access the workflow runtime context storage. */
	//WorkflowRuntimeContextStorage& contextStorage();

 //   /** Access the workflow runtime context storage (const version). */
 //   const WorkflowRuntimeContextStorage& contextStorage() const;

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

    //const QString                       _title;                     /** The title of the workflow, used for displaying the workflow in the UI or for logging purposes. */
    quint64                             _duration;                  /** Stored duration after workflow done. */
    //UniqueWorkflowContext               _initialWorkflowContext;    /** Initial workflow context, used for storing any necessary information for the workflow that needs to be accessed by the tasks in the workflow. */
    //WorkflowRuntimeContextStorage       _contextStorage;            /** Storage for the workflow context, used for passing data between tasks. */
    //UniqueWorkflowResultBase            _result;                    /** Result of the workflow, used for storing the success flag and error message after the workflow is done. It is the responsibility of the concrete workflow to populate this result. */
    SharedOperationContext              _operationContext;          /* Operation context for the workflow, used for storing any necessary information about the operation being performed by the workflow. This can be used for providing additional context to the tasks in the recipe, for example by allowing them to access information about the operation being performed by the workflow. The operation context is stored as a shared pointer, allowing it to be shared between the workflow and the tasks in the recipe, and it can be used for storing any necessary information about the operation being performed by the workflow that needs to be accessed by the tasks in the recipe. */
    DoneCallback                        _doneCallback;              /** Optional callback that can be set to be called when the workflow is done, with a success flag and an error message. This can be used by external code to react to the completion of the workflow, for example by showing a success message or an error dialog to the user. */
    WorkflowPlan                        _workflowPlan;              /** Plan for the workflow, used for defining the sequence of tasks and their dependencies. */
};

using UniqueWorkflow = std::unique_ptr<Workflow>;

} // namespace mv::util