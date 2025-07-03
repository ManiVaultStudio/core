// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectManager.h"
#include "Archiver.h"
#include "PluginManagerDialog.h"
#include "ProjectSettingsDialog.h"
#include "NewProjectDialog.h"

#include <CoreInterface.h>
#include <ProjectMetaAction.h>

#include <ModalTask.h>
#include <ModalTaskHandler.h>

#include <util/Exception.h>
#include <util/Serialization.h>

#include <widgets/FileDialog.h>

#include <QStandardPaths>
#include <QGridLayout>
#include <QEventLoop>

#include <exception>

#ifdef _DEBUG
    //#define PROJECT_MANAGER_VERBOSE
#endif

using namespace mv::util;
using namespace mv::gui;

namespace mv
{

ProjectManager::ProjectManager(QObject* parent) :
    AbstractProjectManager(parent),
    _newBlankProjectAction(nullptr, "Blank"),
    _newProjectFromWorkspaceAction(nullptr, "From Workspace..."),
    _openProjectAction(nullptr, "Open Project"),
    _importProjectAction(nullptr, "Import Project"),
    _saveProjectAction(nullptr, "Save Project"),
    _saveProjectAsAction(nullptr, "Save Project As..."),
    _editProjectSettingsAction(nullptr, "Project Settings..."),
    _recentProjectsAction(nullptr, getSettingsPrefix() + "RecentProjects"),
    _publishAction(nullptr, "Publish"),
    _pluginManagerAction(nullptr, "Plugin Browser..."),
    _showStartPageAction(nullptr, "Start Page...", true),
    _backToProjectAction(nullptr, "Back to project")
{
    //_newBlankProjectAction.setShortcut(QKeySequence("Ctrl+B"));
    //_newBlankProjectAction.setShortcutContext(Qt::ApplicationShortcut);
    _newBlankProjectAction.setIconByName("file");
    _newBlankProjectAction.setToolTip("Create project without view plugins and data");

    //_newProjectFromWorkspaceAction.setShortcut(QKeySequence("Ctrl+N"));
    //_newProjectFromWorkspaceAction.setShortcutContext(Qt::ApplicationShortcut);
    _newProjectFromWorkspaceAction.setIcon(workspaces().getIcon());
    _newProjectFromWorkspaceAction.setToolTip("Create new project from workspace");

    //_openProjectAction.setShortcut(QKeySequence("Ctrl+O"));
    //_openProjectAction.setShortcutContext(Qt::ApplicationShortcut);
    _openProjectAction.setIconByName("folder-open");
    _openProjectAction.setToolTip("Open project from disk");

    //_importProjectAction.setShortcut(QKeySequence("Ctrl+I"));
    //_importProjectAction.setShortcutContext(Qt::ApplicationShortcut);
    _importProjectAction.setIconByName("file-import");
    _importProjectAction.setToolTip("Import project from disk");

    //_saveProjectAction.setShortcut(QKeySequence("Ctrl+S"));
    //_saveProjectAction.setShortcutContext(Qt::ApplicationShortcut);
    _saveProjectAction.setIconByName("floppy-disk");
    _saveProjectAction.setToolTip("Save project to disk");

    //_saveProjectAsAction.setShortcut(QKeySequence("Ctrl+Shift+S"));
    //_saveProjectAsAction.setShortcutContext(Qt::ApplicationShortcut);
    _saveProjectAsAction.setIconByName("floppy-disk");
    _saveProjectAsAction.setToolTip("Save project to disk in a chosen location");

    //_editProjectSettingsAction.setShortcut(QKeySequence("Ctrl+Shift+P"));
    //_editProjectSettingsAction.setShortcutContext(Qt::ApplicationShortcut);
    _editProjectSettingsAction.setIconByName("gear");

    _newProjectMenu.setIcon(StyledIcon("file"));
    _newProjectMenu.setTitle("New Project");
    _newProjectMenu.setToolTip("Create new project");
    _newProjectMenu.addAction(&_newBlankProjectAction);
    //_newProjectMenu.addAction(&_newProjectFromWorkspaceAction);

    _importDataMenu.setIcon(StyledIcon("file-import"));
    _importDataMenu.setTitle("Import data...");
    _importDataMenu.setToolTip("Import data into ManiVault");

    //_publishAction.setShortcut(QKeySequence("Ctrl+P"));
    //_publishAction.setShortcutContext(Qt::ApplicationShortcut);
    _publishAction.setIconByName("cloud-arrow-up");
    _publishAction.setToolTip("Publish the ManiVault application");

    //_pluginManagerAction.setShortcut(QKeySequence("Ctrl+M"));
    //_pluginManagerAction.setShortcutContext(Qt::ApplicationShortcut);
    _pluginManagerAction.setIconByName("plug");
    _pluginManagerAction.setToolTip("View loaded plugins");

    //_showStartPageAction.setShortcut(QKeySequence("Alt+W"));
    //_showStartPageAction.setShortcutContext(Qt::ApplicationShortcut);
    _showStartPageAction.setIconByName("door-open");
    _showStartPageAction.setToolTip("Show the ManiVault start page");
    //_showStartPageAction.setChecked(!Application::current()->shouldOpenProjectAtStartup());

    //_backToProjectAction.setShortcut(QKeySequence("Alt+W"));
    //_backToProjectAction.setShortcutContext(Qt::ApplicationShortcut);
    _backToProjectAction.setIconByName("reply");
    _backToProjectAction.setToolTip("Go back to the current project");
    _backToProjectAction.setDefaultWidgetFlags(TriggerAction::Icon);
    //_backToProjectAction.setChecked(!Application::current()->shouldOpenProjectAtStartup());

    auto mainWindow = Application::topLevelWidgets().first();

    mainWindow->addAction(&_newProjectFromWorkspaceAction);
    mainWindow->addAction(&_openProjectAction);
    mainWindow->addAction(&_importProjectAction);
    mainWindow->addAction(&_saveProjectAction);
    mainWindow->addAction(&_showStartPageAction);

    connect(&_newBlankProjectAction, &QAction::triggered, this, [this]() -> void {
        newBlankProject();
    });

    connect(&_newProjectFromWorkspaceAction, &QAction::triggered, this, [this]() -> void {
        auto* dialog = new NewProjectDialog();
        connect(dialog, &NewProjectDialog::finished, dialog, &NewProjectDialog::deleteLater);
        dialog->open();
    });

    connect(&_openProjectAction, &QAction::triggered, this, [this]() -> void {
        openProject();
    });

    connect(&_importProjectAction, &QAction::triggered, this, [this]() -> void {
        importProject();
    });

    connect(&_saveProjectAction, &QAction::triggered, [this]() -> void {
        if (_project.isNull())
            return;

        if (_project->getStudioModeAction().isChecked()) {
            _project->setStudioMode(false);
            {
                saveProject(_project->getFilePath());
            }
            _project->setStudioMode(true);
        }
        else {
            saveProject(_project->getFilePath());
        }
    });

    connect(&_saveProjectAsAction, &QAction::triggered, [this]() -> void {
        if (_project.isNull())
            return;

        saveProjectAs();
    });

    connect(&_editProjectSettingsAction, &TriggerAction::triggered, this, [this](bool checked) -> void {
        auto* dialog = new ProjectSettingsDialog();
        connect(dialog, &ProjectSettingsDialog::finished, dialog, &ProjectSettingsDialog::deleteLater);
        dialog->open();
    });

    connect(&_importDataMenu, &QMenu::aboutToShow, this, [this]() -> void {
        _importDataMenu.clear();

        for (auto& pluginTriggerAction : plugins().getPluginTriggerActions(plugin::Type::LOADER))
            _importDataMenu.addAction(pluginTriggerAction);

        _importDataMenu.setEnabled(!_importDataMenu.actions().isEmpty());
    });

    connect(&_pluginManagerAction, &TriggerAction::triggered, this, [this](bool checked) -> void {
        PluginManagerDialog::create();
    });

    const auto updateActionsReadOnly = [this]() -> void {
        _saveProjectAction.setEnabled(hasProject());
        _saveProjectAsAction.setEnabled(hasProject());
        _saveProjectAsAction.setEnabled(hasProject() && !_project->getFilePath().isEmpty());
        _editProjectSettingsAction.setEnabled(hasProject());
        _importProjectAction.setEnabled(hasProject());
        _importDataMenu.setEnabled(hasProject());
        _pluginManagerAction.setEnabled(hasProject());
        _publishAction.setEnabled(hasProject());
        _backToProjectAction.setVisible(hasProject());
    };

    connect(this, &ProjectManager::projectCreated, this, updateActionsReadOnly);
    connect(this, &ProjectManager::projectDestroyed, this, updateActionsReadOnly);
    connect(this, &ProjectManager::projectOpened, this, updateActionsReadOnly);
    connect(this, &ProjectManager::projectSaved, this, updateActionsReadOnly);

    updateActionsReadOnly();

    _recentProjectsAction.initialize("Manager/Project/Recent", "Project", "Ctrl");

    connect(&_recentProjectsAction, &RecentFilesAction::triggered, this, [this](const QString& filePath) -> void {
        openProject(filePath);
    });

    connect(&_publishAction, &TriggerAction::triggered, this, [this]() -> void {
        publishProject();
    });

    connect(&_backToProjectAction, &TriggerAction::triggered, this, [this]() -> void {
        mv::help().getShowLearningCenterPageAction().setChecked(false);
        getShowStartPageAction().setChecked(false);
    });
}

ProjectManager::~ProjectManager()
{
    reset();
}

void ProjectManager::initialize()
{
#ifdef PROJECT_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    AbstractProjectManager::initialize();

    if (isInitialized())
        return;

    beginInitialization();
    {
        _projectsTreeModel.populateFromPluginDsns();
    }
    endInitialization();
}

void ProjectManager::reset()
{
#ifdef PROJECT_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    beginReset();
    {
        if (!isCoreDestroyed()) {
            actions().reset();
            dataHierarchy().reset();
            data().reset();
            plugins().reset();
        }
    }
    endReset();
}

void ProjectManager::newProject(const QString& workspaceFilePath /*= ""*/)
{
    try
    {
#ifdef PROJECT_MANAGER_VERBOSE
        qDebug() << __FUNCTION__;
#endif

        createProject();

        if (QFileInfo(workspaceFilePath).exists())
            workspaces().loadWorkspace(workspaceFilePath);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to create new project", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to create new project");
    }
}

void ProjectManager::newProject(const Qt::AlignmentFlag& alignment, bool logging /*= false*/)
{
    newBlankProject();

    auto dockAreaFlag = DockAreaFlag::Right;

    switch (alignment) {
        case Qt::AlignLeft:
            dockAreaFlag = DockAreaFlag::Left;
            break;

        case Qt::AlignRight:
            dockAreaFlag = DockAreaFlag::Right;
            break;

        default:
            break;
    }

    plugin::ViewPlugin* dataHierarchyPlugin = nullptr;

    if (plugins().isPluginLoaded("Data hierarchy"))
        dataHierarchyPlugin = plugins().requestViewPlugin("Data hierarchy", nullptr, dockAreaFlag);

    if (plugins().isPluginLoaded("Data properties"))
        plugins().requestViewPlugin("Data properties", dataHierarchyPlugin, DockAreaFlag::Bottom);

    if (logging && plugins().isPluginLoaded("Logging"))
        plugins().requestViewPlugin("Logging", nullptr, DockAreaFlag::Bottom);
}

void ProjectManager::newBlankProject()
{
    try
    {
#ifdef PROJECT_MANAGER_VERBOSE
        qDebug() << __FUNCTION__;
#endif

        createProject();

        workspaces().reset();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to create blank project", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to create blank project");
    }
}

void ProjectManager::openProject(QString filePath /*= ""*/, bool importDataOnly /*= false*/, bool loadWorkspace /*= true*/)
{
    try
    {
#ifdef PROJECT_MANAGER_VERBOSE
        qDebug() << __FUNCTION__ << filePath;
#endif

        const auto scopedState = ScopedState(this, State::OpeningProject);

        if (QFileInfo(filePath).isDir())
            throw std::runtime_error("Project file path may not be a directory");

        QTemporaryDir temporaryDirectory(QDir::cleanPath(Application::current()->getTemporaryDir().path() + QDir::separator() + "OpenProject"));

        setTemporaryDirPath(TemporaryDirType::Open, temporaryDirectory.path());

        const auto temporaryDirectoryPath = temporaryDirectory.path();

        Application::setSerializationAborted(false);

        ToggleAction disableReadOnlyAction(this, "Allow edit of published project");

        if (filePath.isEmpty()) {
            FileOpenDialog fileOpenDialog;

            fileOpenDialog.setWindowTitle("Open ManiVault Project");
            fileOpenDialog.setNameFilters({ "ManiVault project files (*.mv)" });
            fileOpenDialog.setDefaultSuffix(".mv");
            fileOpenDialog.setDirectory(Application::current()->getSetting("Projects/WorkingDirectory", QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)).toString());
            fileOpenDialog.setOption(QFileDialog::DontUseNativeDialog, true);

            StringAction    titleAction(this, "Title");
            StringAction    descriptionAction(this, "Description");
            StringAction    tagsAction(this, "Tags");
            StringAction    commentsAction(this, "Comments");
            StringAction    contributorsAction(this, "Contributors");

            titleAction.setEnabled(false);
            descriptionAction.setEnabled(false);
            tagsAction.setEnabled(false);
            commentsAction.setEnabled(false);
            contributorsAction.setEnabled(false);
            disableReadOnlyAction.setEnabled(false);

            auto fileDialogLayout   = dynamic_cast<QGridLayout*>(fileOpenDialog.layout());
            auto rowCount           = fileDialogLayout->rowCount();

            fileDialogLayout->addWidget(titleAction.createLabelWidget(&fileOpenDialog), rowCount, 0);
            fileDialogLayout->addWidget(titleAction.createWidget(&fileOpenDialog), rowCount, 1, 1, 2);

            fileDialogLayout->addWidget(descriptionAction.createLabelWidget(&fileOpenDialog), rowCount + 1, 0);
            fileDialogLayout->addWidget(descriptionAction.createWidget(&fileOpenDialog), rowCount + 1, 1, 1, 2);

            fileDialogLayout->addWidget(tagsAction.createLabelWidget(&fileOpenDialog), rowCount + 2, 0);
            fileDialogLayout->addWidget(tagsAction.createWidget(&fileOpenDialog), rowCount + 2, 1, 1, 2);

            fileDialogLayout->addWidget(commentsAction.createLabelWidget(&fileOpenDialog), rowCount + 3, 0);
            fileDialogLayout->addWidget(commentsAction.createWidget(&fileOpenDialog), rowCount + 3, 1, 1, 2);

            fileDialogLayout->addWidget(contributorsAction.createLabelWidget(&fileOpenDialog), rowCount + 4, 0);
            fileDialogLayout->addWidget(contributorsAction.createWidget(&fileOpenDialog), rowCount + 4, 1, 1, 2);
   
            fileDialogLayout->addWidget(disableReadOnlyAction.createWidget(&fileOpenDialog), rowCount + 5, 1, 1, 2);

            connect(&fileOpenDialog, &QFileDialog::currentChanged, this, [&](const QString& filePath) -> void {
                if (!QFileInfo(filePath).isFile())
                    return;

                const auto projectMetaAction = Project::getProjectMetaActionFromProjectFilePath(filePath);

                if (projectMetaAction.isNull())
                    return;

                titleAction.setString(projectMetaAction->getTitleAction().getString());
                descriptionAction.setString(projectMetaAction->getDescriptionAction().getString());
                tagsAction.setString(projectMetaAction->getTagsAction().getStrings().join(", "));
                commentsAction.setString(projectMetaAction->getCommentsAction().getString());
                contributorsAction.setString(projectMetaAction->getContributorsAction().getStrings().join(","));
                disableReadOnlyAction.setEnabled(projectMetaAction->getReadOnlyAction().isChecked());
            });

            fileOpenDialog.open();

            QEventLoop eventLoop;
            
            connect(&fileOpenDialog, &QDialog::finished, &eventLoop, &QEventLoop::quit);
            
            eventLoop.exec();

            if (fileOpenDialog.result() != QDialog::Accepted)
                return;

            if (fileOpenDialog.selectedFiles().count() != 1)
                throw std::runtime_error("Only one file may be selected");

            filePath = fileOpenDialog.selectedFiles().first();

            Application::current()->setSetting("Projects/WorkingDirectory", QFileInfo(filePath).absolutePath());
        }

        qDebug().noquote() << "Open ManiVault project from" << filePath;

        workspaces().reset();

        if (!importDataOnly)
            newProject();

		emit projectAboutToBeOpened(*_project);
	    {
            _project->setFilePath(filePath);

            auto& projectSerializationTask      = projects().getProjectSerializationTask();
            auto& compressionTask               = projectSerializationTask.getCompressionTask();

            projectSerializationTask.startLoad(filePath);

            Archiver archiver;

            const QFileInfo workspaceFileInfo(temporaryDirectoryPath, "workspace.json");

            archiver.extractSingleFile(filePath, "workspace.json", QFileInfo(temporaryDirectoryPath, "workspace.json").absoluteFilePath());
            
            compressionTask.setSubtasks(archiver.getTaskNamesForDecompression(filePath));
            compressionTask.setRunning();

            connect(&archiver, &Archiver::taskStarted, this, [this, &compressionTask](const QString& taskName) -> void {
                compressionTask.setSubtaskStarted(taskName, QString("Extracting: %1").arg(taskName));

                QCoreApplication::processEvents();
            });

            connect(&archiver, &Archiver::taskFinished, this, [this, &compressionTask](const QString& taskName) -> void {
                compressionTask.setSubtaskFinished(taskName, QString("Extracting: %1").arg(taskName));

                QCoreApplication::processEvents();
            });

            connect(&projectSerializationTask, &Task::requestAbort, this, [this]() -> void {
                Application::setSerializationAborted(true);

                throw std::runtime_error("Canceled before project was loaded");
            });

            archiver.decompress(filePath, temporaryDirectoryPath);

            compressionTask.setFinished();

            projects().fromJsonFile(QFileInfo(temporaryDirectoryPath, "project.json").absoluteFilePath());
            
            if (loadWorkspace) {
                if (workspaceFileInfo.exists())
                    workspaces().loadWorkspace(workspaceFileInfo.absoluteFilePath(), false);

                workspaces().setWorkspaceFilePath("");
            }

            _recentProjectsAction.addRecentFilePath(filePath);

            _project->updateContributors();

            if (disableReadOnlyAction.isEnabled() && disableReadOnlyAction.isChecked())
                _project->getReadOnlyAction().setChecked(false);

            if (_project->isStartupProject())
                ModalTask::getGlobalHandler()->setEnabled(true);

            if (_project->getOverrideApplicationStatusBarAction().isChecked() && _project->getStatusBarVisibleAction().isChecked())
            {
                auto& miscellaneousSettings = mv::settings().getMiscellaneousSettings();

                miscellaneousSettings.getStatusBarVisibleAction().setChecked(_project->getStatusBarVisibleAction().isChecked());

                /* TODO: Fix plugin status bar action visibility
            	miscellaneousSettings.getStatusBarOptionsAction().setSelectedOptions(_project->getStatusBarOptionsAction().getSelectedOptions());
                */
            }

            unsetTemporaryDirPath(TemporaryDirType::Open);

            qDebug().noquote() << filePath << "loaded successfully";
        }
        emit projectOpened(*_project);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to load ManiVault project", e);

        projects().getProjectSerializationTask().setFinished();
    }
    catch (...)
    {
        exceptionMessageBox("Unable to load ManiVault project");

        projects().getProjectSerializationTask().setFinished();
    }
}

void ProjectManager::openProject(QUrl url, const QString& targetDirectory /*= ""*/, bool importDataOnly /*= false*/, bool loadWorkspace /*= false*/)
{
    try {
   //     if (hasProject())
			//saveProjectAs();

        if (url.isLocalFile()) {
            mv::projects().openProject(url.toString());
        } else {
            auto* projectDownloader = new FileDownloader(FileDownloader::StorageMode::All, Task::GuiScope::Modal);

            projectDownloader->setTargetDirectory(targetDirectory);

            connect(projectDownloader, &FileDownloader::downloaded, this, [projectDownloader]() -> void {
                mv::projects().openProject(projectDownloader->getDownloadedFilePath());
                projectDownloader->deleteLater();
            });

            connect(projectDownloader, &FileDownloader::aborted, this, [projectDownloader]() -> void {
                qDebug() << "Download aborted by user";
            });

            projectDownloader->download(url);
        }
	}
	catch (std::exception& e)
	{
	    exceptionMessageBox("Unable to load ManiVault project", e);
	}
	catch (...)
	{
	    exceptionMessageBox("Unable to load ManiVault project");
	}
}

void ProjectManager::importProject(QString filePath /*= ""*/)
{
    try
    {
#ifdef PROJECT_MANAGER_VERBOSE
        qDebug() << __FUNCTION__ << filePath;
#endif

        const auto scopedState = ScopedState(this, State::ImportingProject);

        emit projectAboutToBeImported(filePath);
        {
            openProject(filePath, true, false);
        }
        emit projectImported(filePath);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to import project", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to import project");
    }
}

void ProjectManager::saveProject(QString filePath /*= ""*/, const QString& password /*= ""*/)
{
    try
    {
#ifdef PROJECT_MANAGER_VERBOSE
        qDebug() << __FUNCTION__ << filePath;
#endif

        const auto scopedState = ScopedState(this, State::SavingProject);

        emit projectAboutToBeSaved(*_project);
        {
            if (QFileInfo(filePath).isDir())
                throw std::runtime_error("Project file path may not be a directory");

            QTemporaryDir temporaryDirectory(QDir::cleanPath(Application::current()->getTemporaryDir().path() + QDir::separator() + "SaveProject"));

            const auto temporaryDirectoryPath = temporaryDirectory.path();

            setTemporaryDirPath(TemporaryDirType::Save, temporaryDirectory.path());

            if (filePath.isEmpty()) {

                FileSaveDialog saveFileDialog;

                saveFileDialog.setWindowTitle("Save ManiVault Project");
                saveFileDialog.setNameFilters({ "ManiVault project files (*.mv)" });
                saveFileDialog.setDefaultSuffix(".mv");
                saveFileDialog.setDirectory(Application::current()->getSetting("Projects/WorkingDirectory", QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)).toString());

                auto fileDialogLayout   = dynamic_cast<QGridLayout*>(saveFileDialog.layout());
                auto rowCount           = fileDialogLayout->rowCount();

                QCheckBox   passwordProtectedCheckBox("Password protected");
                QLineEdit   passwordLineEdit;

                passwordProtectedCheckBox.setChecked(false);
                passwordLineEdit.setPlaceholderText("Enter encryption password...");

                auto compressionLayout = new QHBoxLayout();

                compressionLayout->addWidget(_project->getCompressionAction().getEnabledAction().createWidget(&saveFileDialog));
                compressionLayout->addWidget(_project->getCompressionAction().getLevelAction().createWidget(&saveFileDialog), 1);
                
                fileDialogLayout->addLayout(compressionLayout, rowCount, 1, 1, 2);
                
                //fileDialogLayout->addWidget(&passwordProtectedCheckBox, ++rowCount, 0);
                //fileDialogLayout->addWidget(&passwordLineEdit, rowCount, 1);

                auto& titleAction = _project->getTitleAction();

                fileDialogLayout->addWidget(titleAction.createLabelWidget(nullptr), rowCount + 2, 0);

                GroupAction settingsGroupAction(this, "Settings");

                settingsGroupAction.setIconByName("gear");
                settingsGroupAction.setToolTip("Edit project settings");
                settingsGroupAction.setPopupSizeHint(QSize(420, 320));
                settingsGroupAction.setLabelSizingType(GroupAction::LabelSizingType::Auto);

                settingsGroupAction.addAction(&_project->getTitleAction());
                settingsGroupAction.addAction(&_project->getDescriptionAction());
                settingsGroupAction.addAction(&_project->getTagsAction());
                settingsGroupAction.addAction(&_project->getCommentsAction());

                auto titleLayout = new QHBoxLayout();

                titleLayout->addWidget(titleAction.createWidget(&saveFileDialog));
                titleLayout->addWidget(settingsGroupAction.createCollapsedWidget(&saveFileDialog));

                fileDialogLayout->addLayout(titleLayout, rowCount + 2, 1, 1, 2);

                //const auto updatePassword = [&]() -> void {
                //    passwordLineEdit.setEnabled(passwordProtectedCheckBox.isChecked());
                //};

                //connect(&passwordProtectedCheckBox, &QCheckBox::toggled, this, updatePassword);

                //updatePassword();

                connect(&saveFileDialog, &QFileDialog::currentChanged, this, [this](const QString& filePath) -> void {
                    if (!QFileInfo(filePath).isFile())
                        return;

                    const auto projectMetaAction = Project::getProjectMetaActionFromProjectFilePath(filePath);

                    if (projectMetaAction.isNull())
                        return;

                    _project->getCompressionAction().getEnabledAction().setChecked(projectMetaAction->getCompressionAction().getEnabledAction().isChecked());
                    _project->getCompressionAction().getLevelAction().setValue(projectMetaAction->getCompressionAction().getLevelAction().getValue());
                });

                saveFileDialog.open();

                QEventLoop eventLoop;
                QObject::connect(&saveFileDialog, &QDialog::finished, &eventLoop, &QEventLoop::quit);
                eventLoop.exec();

                if (saveFileDialog.result() != QDialog::Accepted)
                    return;

                if (saveFileDialog.selectedFiles().count() != 1)
                    throw std::runtime_error("Only one file may be selected");

                filePath = saveFileDialog.selectedFiles().first();

                Application::current()->setSetting("Projects/WorkingDirectory", QFileInfo(filePath).absolutePath());
            }
            
            if (filePath.isEmpty() || QFileInfo(filePath).isDir())
                return;

            if (_project->getCompressionAction().getEnabledAction().isChecked())
                qDebug().noquote() << "Saving ManiVault project to" << filePath << "with compression level" << _project->getCompressionAction().getLevelAction().getValue();
            else
                qDebug().noquote() << "Saving ManiVault project to" << filePath << "without compression";

            auto& projectSerializationTask  = projects().getProjectSerializationTask();
            auto& compressionTask           = projectSerializationTask.getCompressionTask();

            projectSerializationTask.startSave(filePath);
            projectSerializationTask.setRunning();

            QCoreApplication::processEvents();

            Archiver archiver;

            connect(&projectSerializationTask, &Task::requestAbort, this, [this]() -> void {
                Application::setSerializationAborted(true);

                throw std::runtime_error("Canceled before project was saved");
            });

            QFileInfo projectJsonFileInfo(temporaryDirectoryPath, "project.json"), projectMetaJsonFileInfo(temporaryDirectoryPath, "meta.json");

            Application::setSerializationAborted(false);

            projects().toJsonFile(projectJsonFileInfo.absoluteFilePath());

            _project->getProjectMetaAction().toJsonFile(projectMetaJsonFileInfo.absoluteFilePath());
            
            QFileInfo workspaceFileInfo(temporaryDirectoryPath, "workspace.json");

            workspaces().saveWorkspace(workspaceFileInfo.absoluteFilePath(), false);

            compressionTask.setSubtasks(archiver.getTaskNamesForDirectoryCompression(temporaryDirectoryPath));
            compressionTask.setRunning();

            connect(&archiver, &Archiver::taskStarted, this, [&compressionTask](const QString& taskName) -> void {
                compressionTask.setSubtaskStarted(taskName, QString("Compressing %1").arg(taskName));
            });

            connect(&archiver, &Archiver::taskFinished, this, [&compressionTask](const QString& taskName) -> void {
                compressionTask.setSubtaskFinished(taskName, QString("Compressing %1").arg(taskName));
            });

            archiver.compressDirectory(temporaryDirectoryPath, filePath, true, _project->getCompressionAction().getEnabledAction().isChecked() ? _project->getCompressionAction().getLevelAction().getValue() : 0, password);

            compressionTask.setFinished();

            _recentProjectsAction.addRecentFilePath(filePath);

            _project->setFilePath(filePath);

            unsetTemporaryDirPath(TemporaryDirType::Save);

            qDebug().noquote() << filePath << "saved successfully";
        }
        emit projectSaved(*_project);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to save project", e);

        projects().getProjectSerializationTask().setFinished();
    }
    catch (...)
    {
        exceptionMessageBox("Unable to save project");

        projects().getProjectSerializationTask().setFinished();
    }
}

void ProjectManager::saveProjectAs()
{
    saveProject("");
}

void ProjectManager::publishProject(QString filePath /*= ""*/)
{
    try
    {
#ifdef PROJECT_MANAGER_VERBOSE
        qDebug() << __FUNCTION__ << filePath;
#endif

        if (!hasProject())
            return;

        const auto scopedState = ScopedState(this, State::PublishingProject);

        /*
        auto& readOnlyAction        = getCurrentProject()->getReadOnlyAction();
        auto& splashScreenAction    = getCurrentProject()->getSplashScreenAction();

        readOnlyAction.cacheState();
        splashScreenAction.cacheState();
        
        readOnlyAction.setChecked(true);
        splashScreenAction.getEnabledAction().setChecked(true);
        */

        emit projectAboutToBePublished(*_project);
        {
            if (QFileInfo(filePath).isDir())
                throw std::runtime_error("Project file path may not be a directory");

            QTemporaryDir temporaryDirectory(QDir::cleanPath(Application::current()->getTemporaryDir().path() + QDir::separator() + "PublishProject"));

            setTemporaryDirPath(TemporaryDirType::Publish, temporaryDirectory.path());

            const auto temporaryDirectoryPath = temporaryDirectory.path();

            auto currentProject = getCurrentProject();

            currentProject->getOverrideApplicationStatusBarAction().cacheState();

            /* TODO: Fix plugin status bar action visibility
            currentProject->getStatusBarVisibleAction().cacheState();
            currentProject->getStatusBarOptionsAction().cacheState();
            */

            currentProject->getOverrideApplicationStatusBarAction().setChecked(true);

            /* TODO: Fix plugin status bar action visibility
            currentProject->getStatusBarVisibleAction().setChecked(true);
            currentProject->getStatusBarOptionsAction().setSelectedOptions({ "Logging", "Background Tasks", "Foreground Tasks" });
            */

            ToggleAction    passwordProtectedAction(this, "Password Protected");
            StringAction    passwordAction(this, "Password");

            if (filePath.isEmpty()) {

                FileSaveDialog saveFileDialog;

                saveFileDialog.setWindowIcon(StyledIcon("cloud-arrow-up"));
                saveFileDialog.setWindowTitle("Publish ManiVault Project");
                saveFileDialog.setNameFilters({ "ManiVault project files (*.mv)" });
                saveFileDialog.setDefaultSuffix(".mv");
                saveFileDialog.setDirectory(Application::current()->getSetting("Projects/WorkingDirectory", QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)).toString());

                auto fileDialogLayout   = dynamic_cast<QGridLayout*>(saveFileDialog.layout());
                auto rowCount           = fileDialogLayout->rowCount();

                QStringList options{ "Compression", "Title" };

                if (options.contains("Password")) {
                    passwordProtectedAction.setToolTip("Whether to password-protect the project");

                    passwordAction.setToolTip("Project password");
                    passwordAction.setPlaceHolderString("Enter encryption password...");
                    passwordAction.setClearable(true);

                    auto passwordLayout = new QHBoxLayout();

                    passwordLayout->addWidget(passwordProtectedAction.createWidget(&saveFileDialog));
                    passwordLayout->addWidget(passwordAction.createWidget(&saveFileDialog), 1);

                    const auto updatePasswordActionReadOnly = [&]() -> void {
                        passwordAction.setEnabled(passwordProtectedAction.isChecked());
                    };

                    connect(&passwordProtectedAction, &ToggleAction::toggled, this, updatePasswordActionReadOnly);

                    updatePasswordActionReadOnly();

                    fileDialogLayout->addLayout(passwordLayout, rowCount + 3, 1, 1, 2);
                }

                if (options.contains("Compression")) {
                    auto compressionLayout = new QHBoxLayout();

                    compressionLayout->addWidget(currentProject->getCompressionAction().getEnabledAction().createWidget(&saveFileDialog));
                    compressionLayout->addWidget(currentProject->getCompressionAction().getLevelAction().createWidget(&saveFileDialog), 1);

                    fileDialogLayout->addLayout(compressionLayout, rowCount, 1, 1, 2);
                }
                    
                GroupAction settingsGroupAction(this, "Settings");

                if (options.contains("Title")) {
                    auto& titleAction = currentProject->getTitleAction();

                    fileDialogLayout->addWidget(titleAction.createLabelWidget(nullptr), rowCount + 2, 0);

                    settingsGroupAction.setIconByName("gear");
                    settingsGroupAction.setToolTip("Edit project settings");
                    settingsGroupAction.setPopupSizeHint(QSize(420, 0));
                    settingsGroupAction.setLabelSizingType(GroupAction::LabelSizingType::Auto);

                    settingsGroupAction.addAction(&currentProject->getTitleAction());
                    settingsGroupAction.addAction(&currentProject->getDescriptionAction());
                    settingsGroupAction.addAction(&currentProject->getTagsAction());
                    settingsGroupAction.addAction(&currentProject->getCommentsAction());
                    settingsGroupAction.addAction(&currentProject->getSplashScreenAction());
                    settingsGroupAction.addAction(&currentProject->getOverrideApplicationStatusBarAction());
                    settingsGroupAction.addAction(&currentProject->getStatusBarVisibleAction());

                    /* TODO: Fix plugin status bar action visibility
                    settingsGroupAction.addAction(&currentProject->getStatusBarOptionsAction());
                    */

                    auto titleLayout = new QHBoxLayout();

                    titleLayout->addWidget(titleAction.createWidget(&saveFileDialog));
                    titleLayout->addWidget(settingsGroupAction.createCollapsedWidget(&saveFileDialog));

                    fileDialogLayout->addLayout(titleLayout, rowCount + 2, 1, 1, 2);
                }
                    
                connect(&saveFileDialog, &QFileDialog::currentChanged, this, [this, currentProject](const QString& filePath) -> void {
                    if (!QFileInfo(filePath).isFile())
                        return;

                    const auto projectMetaAction = Project::getProjectMetaActionFromProjectFilePath(filePath);

                    if (projectMetaAction.isNull())
                        return;

                    currentProject->getCompressionAction().getEnabledAction().setChecked(projectMetaAction->getCompressionAction().getEnabledAction().isChecked());
                    currentProject->getCompressionAction().getLevelAction().setValue(projectMetaAction->getCompressionAction().getLevelAction().getValue());
                });

                saveFileDialog.open();

                QEventLoop eventLoop;
                QObject::connect(&saveFileDialog, &QDialog::finished, &eventLoop, &QEventLoop::quit);
                eventLoop.exec();

                if (saveFileDialog.result() != QDialog::Accepted)
                    return;

                if (saveFileDialog.selectedFiles().count() != 1)
                    throw std::runtime_error("Only one file may be selected");

                filePath = saveFileDialog.selectedFiles().first();

                Application::current()->setSetting("Projects/WorkingDirectory", QFileInfo(filePath).absolutePath());
            }

            if (filePath.isEmpty() || QFileInfo(filePath).isDir())
                return;

            auto& workspaceLockingAction = workspaces().getCurrentWorkspace()->getLockingAction();

            const auto cacheWorkspaceLocked = workspaceLockingAction.isLocked();

            workspaceLockingAction.setLocked(true);
            {
                auto& readOnlyAction            = getCurrentProject()->getReadOnlyAction();
                auto& splashScreenEnabledAction = getCurrentProject()->getSplashScreenAction().getEnabledAction();

                QSignalBlocker readOnlyActionSignalBlocker(&readOnlyAction);
                QSignalBlocker splashScreenEnabledActionSignalBlocker(&splashScreenEnabledAction);

                readOnlyAction.cacheState();
                readOnlyAction.setChecked(true);

                splashScreenEnabledAction.cacheState();
                splashScreenEnabledAction.setChecked(true);

                saveProject(filePath, passwordAction.getString());

                readOnlyAction.restoreState();
                splashScreenEnabledAction.restoreState();
            }
            workspaceLockingAction.setLocked(cacheWorkspaceLocked);

            currentProject->getOverrideApplicationStatusBarAction().restoreState();

			/* TODO: Fix plugin status bar action visibility
            currentProject->getStatusBarVisibleAction().restoreState();
            currentProject->getStatusBarOptionsAction().restoreState();
            */
            unsetTemporaryDirPath(TemporaryDirType::Publish);
        }
        emit projectPublished(*_project);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to publish ManiVault project", e);

        projects().getProjectSerializationTask().setFinished();
    }
    catch (...)
    {
        exceptionMessageBox("Unable to publish ManiVault project");

        projects().getProjectSerializationTask().setFinished();
    }
}

bool ProjectManager::hasProject() const
{
    return getCurrentProject() != nullptr;
}

const mv::Project* ProjectManager::getCurrentProject() const
{
    return _project.get();
}

mv::Project* ProjectManager::getCurrentProject()
{
    return _project.get();
}

QString ProjectManager::extractFileFromManiVaultProject(const QString& maniVaultFilePath, const QTemporaryDir& temporaryDir, const QString& filePath)
{
    const auto temporaryDirectoryPath = temporaryDir.path();

    const QString extractFilePath(filePath);

    QFileInfo extractFileInfo(temporaryDirectoryPath, extractFilePath);

    QString extractedFilePath = "";

    try
    {
        Archiver archiver;

        archiver.extractSingleFile(maniVaultFilePath, extractFilePath, extractFileInfo.absoluteFilePath());

        extractedFilePath = extractFileInfo.absoluteFilePath();
    }
    catch (std::exception& e)
    {
        qDebug() << "Unable to extract file from ManiVault project archive: " << e.what();
    }
    catch (...)
    {
        qDebug() << "Unable to extract file from ManiVault project archive due to an unhandled exception";
    }

    return extractedFilePath;
}

QImage ProjectManager::getWorkspacePreview(const QString& projectFilePath, const QSize& targetSize /*= QSize(500, 500)*/) const
{
    try
    {
        QTemporaryDir temporaryDir(QDir::cleanPath(Application::current()->getTemporaryDir().path() + QDir::separator() + "WorkspacePreview"));

        const auto workspacePreviewFilePath = projects().extractFileFromManiVaultProject(projectFilePath, temporaryDir, "workspace.jpg");

        if (workspacePreviewFilePath.isEmpty())
            return {};

        const auto workspacePreviewImage = QImage(workspacePreviewFilePath);
        
        if (!workspacePreviewImage.isNull())
            return workspacePreviewImage.scaled(targetSize, Qt::KeepAspectRatio);
        
		return {};
    }
    catch (std::exception& e)
    {
        qDebug() << "Unable to get preview image from ManiVault project archive: " << e.what();
    }
    catch (...)
    {
        qDebug() << "Unable to get preview image from ManiVault project archive due to an unhandled exception";
    }

    return {};
}

const ProjectsTreeModel& ProjectManager::getProjectsTreeModel() const
{
    return _projectsTreeModel;
}

QMenu& ProjectManager::getNewProjectMenu()
{
    return _newProjectMenu;
}

QMenu& ProjectManager::getImportDataMenu()
{
    return _importDataMenu;
}

void ProjectManager::createProject()
{
    emit projectAboutToBeCreated();
    {
        data().reset();

        reset();

        _project.reset(new Project());
    }
    emit projectCreated(*_project);

    mv::help().getShowLearningCenterPageAction().setChecked(false);

    _showStartPageAction.setChecked(false);
    
}

void ProjectManager::fromVariantMap(const QVariantMap& variantMap)
{
    _project->fromVariantMap(variantMap);
}

QVariantMap ProjectManager::toVariantMap() const
{
    if (hasProject())
        return _project->toVariantMap();

    return {};
}

}