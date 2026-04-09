// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <util/AbstractWorkflow.h>

#include "ProjectSaveContext.h"

class ProjectSaveWorkflow final : public mv::util::AbstractWorkflow
{
    Q_OBJECT

public:
    explicit ProjectSaveWorkflow(const QString& filePath, QObject* parent = nullptr);

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

public:

    /**
     * Returns a reference to the temporary directory used for any necessary temporary storage during the execution of the workflow. The temporary directory is created using QTemporaryDir, which provides a unique temporary directory that is automatically removed when it goes out of scope. The getTemporaryDir method can be used by derived classes of AbstractWorkflow to access the temporary directory for any necessary temporary storage during the execution of the workflow, for example by using the temporary directory for extracting files or for storing intermediate results that need to be accessed by multiple tasks in the workflow. The getTemporaryDir method returns a reference to the QTemporaryDir object, allowing derived classes to access the path of the temporary directory and to use it for any necessary temporary storage during the execution of the workflow.
     * @return A reference to the temporary directory used for any necessary temporary storage during the execution of the workflow. The temporary directory is created using QTemporaryDir, which provides a unique temporary directory that is automatically removed when it goes out of scope. The getTemporaryDir method can be used by derived classes of AbstractWorkflow to access the temporary directory for any necessary temporary storage during the execution of the workflow, for example by using the temporary directory for extracting files or for storing intermediate results that need to be accessed by multiple tasks in the workflow. The getTemporaryDir method returns a reference to the QTemporaryDir object, allowing derived classes to access the path of the temporary directory and to use it for any necessary temporary storage during the execution of the workflow.
     */
    const QTemporaryDir& getTemporaryDir() const;

    /**
     * Returns the path of the temporary directory used for any necessary temporary storage during the execution of the workflow. The temporary directory is created using QTemporaryDir, which provides a unique temporary directory that is automatically removed when it goes out of scope. The getTemporaryDirPath method can be used by derived classes of AbstractWorkflow to access the path of the temporary directory for any necessary temporary storage during the execution of the workflow, for example by using the temporary directory for extracting files or for storing intermediate results that need to be accessed by multiple tasks in the workflow. The getTemporaryDirPath method returns a QString containing the path of the temporary directory, allowing derived classes to access the path of the temporary directory and to use it for any necessary temporary storage during the execution of the workflow.
     * @return A QString containing the path of the temporary directory used for any necessary temporary storage during the execution of the workflow. The temporary directory is created using QTemporaryDir, which provides a unique temporary directory that is automatically removed when it goes out of scope. The getTemporaryDirPath method can be used by derived classes of AbstractWorkflow to access the path of the temporary directory for any necessary temporary storage during the execution of the workflow, for example by using the temporary directory for extracting files or for storing intermediate results that need to be accessed by multiple tasks in the workflow. The getTemporaryDirPath method returns a QString containing the path of the temporary directory, allowing derived classes to access the path of the temporary directory and to use it for any necessary temporary storage during the execution of the workflow.
     */
    QString getTemporaryDirPath() const;

protected: // Workflow result initialization

    /**
     * Perform any necessary initialization of the workflow result before the workflow is executed. This allows for setting up any necessary information in the result that needs to be accessed after the workflow is done, for example by initializing the workflow result with any necessary information about the result of the workflow that needs to be accessed after the workflow is done. The initResult method will be called with a reference to the workflow result, allowing derived classes to implement custom logic for initializing the workflow result before the workflow is executed.
     * @param result Reference to the workflow result, used for storing the success flag and error message after the workflow is done. The initResult method can be used for performing any necessary initialization of the workflow result before the workflow is executed, for example by setting up any necessary information in the result that needs to be accessed after the workflow is done. The initResult method will be called with a reference to the workflow result, allowing derived classes to implement custom logic for initializing the workflow result before the workflow is executed.
     */
    void initResult(UniqueWorkflowResultBase& result) override;

private:

    /**
     * Utility method for creating a workflow context from a file. This method can be used by derived classes of AbstractWorkflow to implement custom logic for creating a workflow context based on a file, for example by reading the file and extracting any necessary information for the workflow that needs to be accessed by the tasks in the workflow. The createContextFromFile method will take a file path as an argument and return a unique pointer to a WorkflowContextBase that is initialized with the necessary information extracted from the file.
     * @param filePath Path to the file from which to create the workflow context. This method can be used for creating a workflow context based on a file, for example by reading the file and extracting any necessary information for the workflow that needs to be accessed by the tasks in the workflow. The createContextFromFile method can be used by derived classes of AbstractWorkflow to implement custom logic for creating a workflow context based on a file, and it can return a unique pointer to a WorkflowContextBase that is initialized with the necessary information extracted from the file.
     * @return A unique pointer to a WorkflowContextBase that is initialized with the necessary information extracted from the file. This method can be used by derived classes of AbstractWorkflow to implement custom logic for creating a workflow context based on a file, and it can return a unique pointer to a WorkflowContextBase that is initialized with the necessary information extracted from the file.
     */
    static UniqueWorkflowContext createContext(const QString& filePath);

private: // Stages

    void setup(ProjectSaveContext& context);
    void saveProjectJson(ProjectSaveContext& context);
    void saveProjectMetaJson(ProjectSaveContext& context);
	void saveWorkspaceJson(ProjectSaveContext& context);
    void archive(ProjectSaveContext& context);
    void finalize(ProjectSaveContext& context);

private:
    QTemporaryDir   _temporaryDir;
};
