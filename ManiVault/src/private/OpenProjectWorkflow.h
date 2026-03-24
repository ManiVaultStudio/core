// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ProjectLoadRecipeBuilder.h"

#include <util/WorkflowBase.h>

#include <ModalTask.h>

namespace mv
{
    class ProjectManager;
}

struct OpenProjectContext
{
    QString filePath;
    bool loadWorkspace = true;
    bool importDataOnly = false;
    bool disableReadOnly = false;

    std::unique_ptr<QTemporaryDir> temporaryDirectory;
    QString temporaryDirectoryPath;
    QString workspaceJsonPath;
    QString projectJsonPath;

    QString error;
};

class OpenProjectWorkflow final : public mv::util::WorkflowBase<OpenProjectContext>
{
    Q_OBJECT

public:
    explicit OpenProjectWorkflow(mv::ProjectManager& projectManager, QObject* parent = nullptr);

    void setInput(QString filePath, bool loadWorkspace, bool importDataOnly, bool disableReadOnly);

signals:
    void finished(bool success, const QString& errorMessage);

protected:
    void initializeContext(OpenProjectContext& storage) override;

    void onStorageDone(const OpenProjectContext& storage) override;

    void handleDone(QtTaskTree::DoneWith doneWith) override;

    QtTaskTree::Group makeRecipe() override;

private:
    void setupOpenProject(OpenProjectContext& ctx);
    void extractProjectArchive(OpenProjectContext& ctx);
    void loadProjectJson(OpenProjectContext& ctx);
    void loadWorkspaceFromJson(OpenProjectContext& ctx);
    void finalizeOpenProject(OpenProjectContext& ctx);

private:
    mv::ProjectManager&                             _projectManager;
    QString                                         _filePath;
    bool                                            _loadWorkspace = true;
    bool                                            _importDataOnly = false;
    bool                                            _disableReadOnly = false;
    QString                                         _finalError;
    mv::ModalTask                                   _loadTask;
    mv::Task                                        _setupTask;
    mv::Task                                        _extractJsonTask;
    mv::Task                                        _loadDatasetsJsonTask;
    mv::Task                                        _loadWorkspaceJsonTask;
       _dataHierarchyContextStorage;
    ProjectLoadRecipeBuilder                        _projectLoadRecipeBuilder;
};
