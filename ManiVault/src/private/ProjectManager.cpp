// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectManager.h"
#include "PluginManagerDialog.h"
#include "ProjectSettingsDialog.h"
#include "NewProjectDialog.h"
#include "Archiver.h"

#include <CoreInterface.h>
#include <ProjectMetaAction.h>

#include <ModalTask.h>
#include <ModalTaskHandler.h>

#include <models/HardwareSpecTreeModel.h>

#include <util/Exception.h>
#include <util/Serialization.h>
#include <util/StandardPaths.h>


#include <widgets/FileDialog.h>

#include <QGridLayout>
#include <QEventLoop>
#include <QHeaderView>
#include <QFuture>
#include <QFutureWatcher>
#include <QMetaObject>

#include <exception>

#include "Task.h"

#ifdef _DEBUG
    #define PROJECT_MANAGER_VERBOSE
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
    _backToProjectAction(nullptr, "Back to project"),
    _projectsListModel(StandardItemModel::PopulationMode::AutomaticSynchronous, this)
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
            if (pluginTriggerAction->getPluginFactory()->getAllowPluginCreationFromStandardGui())
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

    _recentProjectsAction.initialize("Project", "Ctrl");

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

    getProjectDownloadTask().setMayKill(true);
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
    try {
        if (hasActiveWorkflow())
            throw std::runtime_error("Another workflow is active");

        if (hasProject() && getCurrentProject()->getFilePath() == filePath)
            throw std::runtime_error("Project is already open");

        if (isOpeningProject())
            throw std::runtime_error("Cannot open project while another project is being opened");

        if (filePath.isEmpty()) {
            filePath = chooseProjectFileViaDialog();
            if (filePath.isEmpty())
                return;
        }

        setState(State::OpeningProject);

        auto workflow = std::make_unique<ProjectOpenWorkflow>(filePath);

        //connect(workflow.get(), &ProjectOpenWorkflow::finished, this, [this, filePath, workflowPtr = workflow.get()](bool success, const QString& error) {
        //    setState(State::Idle);

        //    if (success) {
        //        emit projectOpened(*_project);
        //    }

        //    resetActiveWorkflow();
        //});

        setActiveWorkflow(std::move(workflow));
        getActiveWorkflow()->start();
    }
    catch (const std::exception& e) {
        setState(State::Idle);
        exceptionMessageBox("Unable to load ManiVault project", e);
    }
}

void ProjectManager::openProject(QUrl url, const QString& targetDirectory /*= ""*/, bool importDataOnly /*= false*/, bool loadWorkspace /*= false*/)
{
#ifdef PROJECT_MANAGER_VERBOSE
    qDebug() << __FUNCTION__ << url << targetDirectory << importDataOnly << loadWorkspace;
#endif

    try {
        if (url.isLocalFile()) {
            mv::projects().openProject(url.toLocalFile());
            return;
        }

        auto fileNameFuture = resolveProjectFileNameAsync(url);

        auto* fileNameWatcher = new QFutureWatcher<QString>(this);

        connect(fileNameWatcher, &QFutureWatcher<QString>::finished, this,
            [this, fileNameWatcher, url, targetDirectory]() {
                try {
                    const auto fileName = fileNameWatcher->result();

                    fileNameWatcher->deleteLater();

                    const auto downloadedProjectFilePath = getDownloadedProjectsDir().filePath(fileName);

                    const QFileInfo downloadedProjectFileInfo(downloadedProjectFilePath);

                    if (downloadedProjectFileInfo.exists()) {
                        QFuture<bool> staleFuture = isDownloadedProjectStaleAsync(url);

                        auto staleWatcher = new QFutureWatcher<bool>(this);

                        connect(staleWatcher, &QFutureWatcher<bool>::finished, this, [this, staleWatcher, url, fileName, downloadedProjectFilePath, targetDirectory]() {
                            try {
                                Application::requestRemoveOverrideCursor(Qt::WaitCursor, true);

                                if (staleWatcher->future().isCanceled() || !staleWatcher->future().isFinished()) {
                                    qDebug() << "Failed to check if project is stale at" << downloadedProjectFilePath;

                                    try {
                                        mv::projects().openProject(downloadedProjectFilePath);
                                    }
                                    catch (...) {
                                        qWarning() << "Unable to establish whether the project is stale";
                                    }

                                    staleWatcher->deleteLater();
                                    return;
                                }

                                if (staleWatcher->result()) {
                                    QMetaObject::invokeMethod(qApp, [this, fileName, downloadedProjectFilePath, url, targetDirectory]() {
                                        QMessageBox downloadQuestionMessageBox;

                                        downloadQuestionMessageBox.setWindowIcon(StyledIcon("download"));
                                        downloadQuestionMessageBox.setWindowTitle(QString("Updated project available...").arg(fileName));
                                        downloadQuestionMessageBox.setText(QString("An updated version of %1 is available on the server. Do you want to download it?").arg(fileName));
                                        downloadQuestionMessageBox.setIcon(QMessageBox::Warning);

                                        auto yesButton = downloadQuestionMessageBox.addButton("Yes", QMessageBox::AcceptRole);
                                        auto noButton = downloadQuestionMessageBox.addButton("No", QMessageBox::RejectRole);

                                        downloadQuestionMessageBox.setDefaultButton(noButton);
                                        downloadQuestionMessageBox.exec();

                                        if (downloadQuestionMessageBox.clickedButton() == yesButton) {
                                            QFile::remove(downloadedProjectFilePath);
                                            downloadAndOpenProject(url, targetDirectory);
                                        }
                                        else {
                                            mv::projects().openProject(downloadedProjectFilePath);
                                        }
                                        });
                                }
                                else {
                                    qDebug() << "Project is not stale, opening from" << downloadedProjectFilePath;
                                    mv::projects().openProject(downloadedProjectFilePath);
                                }
                            }
                            catch (const std::exception& e) {
                                qCritical() << "Failed while checking stale/opening project from" << url.toString() << ":" << e.what();
                            }

                            staleWatcher->deleteLater();
                        });

                    staleWatcher->setFuture(staleFuture);
                }
                else {
                    downloadAndOpenProject(url, targetDirectory);
                }
            }
            catch (const std::exception& e) {
                qCritical() << "Failed to resolve project filename for" << url.toString() << ":" << e.what();
            }
        });

        fileNameWatcher->setFuture(fileNameFuture);
    }
    catch (std::exception& e) {
        exceptionMessageBox("Unable to open ManiVault project", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to open ManiVault project due to an unhandled exception");
    }
}

void ProjectManager::downloadAndOpenProject(QUrl url, const QString& targetDirectory) const
{
    QFuture<QString> future = mv::projects().downloadProjectAsync(url, targetDirectory);

    auto watcher = new QFutureWatcher<QString>(const_cast<ProjectManager*>(this));

    connect(watcher, &QFutureWatcher<QString>::finished, watcher, [watcher, url]() {
        try {
            Application::requestRemoveOverrideCursor(Qt::WaitCursor, true);

            if (watcher->future().isCanceled() || watcher->future().isFinished() == false)
                throw std::runtime_error("Future is cancelled or did not finish");

            QString projectFilePath = watcher->future().result();

            QMetaObject::invokeMethod(qApp, [projectFilePath]() {
                mv::projects().openProject(projectFilePath);
            });
        }
        catch (const std::exception& e) {
            qCritical() << "Failed to download project from" << url.toString() << ":" << e.what();
        }

        watcher->deleteLater();
    });

    watcher->setFuture(future);
}

QFuture<QString> ProjectManager::resolveProjectFileNameAsync(const QUrl& url)
{
	const auto candidate = url.fileName();

	if (!candidate.isEmpty() && isMvFileName(candidate)) {
		QPromise<QString> promise;

		auto future = promise.future();

		promise.addResult(candidate);
		promise.finish();

		return future;
	}

	// Otherwise use robust resolver (Content Disposition / OSF metadata / etc.)
	return FileDownloader::getFinalFileNameAsync(url);
};

void ProjectManager::openProject(util::ProjectsModelProjectSharedPtr project, const QString& targetDirectory, bool importDataOnly, bool loadWorkspace)
{
#ifdef PROJECT_MANAGER_VERBOSE
    qDebug() << __FUNCTION__ << project->getTitle() << targetDirectory << importDataOnly << loadWorkspace;
#endif

    Application::requestOverrideCursor(Qt::WaitCursor);

    try {
        if (!project)
            throw std::runtime_error("Project is null");

        const auto systemCompatibility  = HardwareSpec::getSystemCompatibility(project->getMinimumHardwareSpec(), project->getRecommendedHardwareSpec());
        const auto notCompatible        = systemCompatibility._compatibility == HardwareSpec::SystemCompatibility::Incompatible;
        const auto notRecommended       = systemCompatibility._compatibility == HardwareSpec::SystemCompatibility::Minimum;

        if (notCompatible || notRecommended) {
            QDialog projectIncompatibleWithSystemDialog;

            projectIncompatibleWithSystemDialog.setWindowIcon(systemCompatibility._icon);
            projectIncompatibleWithSystemDialog.setWindowTitle("Incompatible System");
            projectIncompatibleWithSystemDialog.setMinimumWidth(500);
            projectIncompatibleWithSystemDialog.setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

            auto layout = new QVBoxLayout(&projectIncompatibleWithSystemDialog);
            auto message = new QLabel();

            layout->setSpacing(10);

            message->setWordWrap(true);

            if (notCompatible)
                message->setText("<p>Your system does not meet the minimum requirements for this project, there might be problems with opening it, its stability and performance!</p>");

            if (notRecommended)
                message->setText("<p>Your system does not meet the recommended requirements for this project, the interactivity might not be optimal!</p>");

            layout->addWidget(message);

            auto requirementsLayout = new QVBoxLayout();
            auto models = QList<HardwareSpecTreeModel*>({ new HardwareSpecTreeModel(&projectIncompatibleWithSystemDialog), new HardwareSpecTreeModel(&projectIncompatibleWithSystemDialog) });
            auto treeViews = QList<QTreeView*>({ new QTreeView(), new QTreeView() });
            auto recommendedCheckBox = new QCheckBox("Show recommended");

            recommendedCheckBox->setChecked(notRecommended);

            requirementsLayout->setSpacing(5);

            models.first()->setHardwareSpec(project->getMinimumHardwareSpec());
            models.first()->setHorizontalHeaderLabels({ "Component", "System", "Minimum" });

            models.last()->setHardwareSpec(project->getRecommendedHardwareSpec());
            models.last()->setHorizontalHeaderLabels({ "Component", "System", "Recommended" });

            for (auto treeView : treeViews) {
                treeView->setIconSize(QSize(13, 13));
                treeView->setModel(models[treeViews.indexOf(treeView)]);
                treeView->expandAll();

                treeView->header()->setStretchLastSection(false);
                treeView->header()->setSectionResizeMode(QHeaderView::Interactive);
                treeView->header()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);
                treeView->header()->setSectionResizeMode(1, QHeaderView::ResizeMode::Fixed);
                treeView->header()->setSectionResizeMode(2, QHeaderView::ResizeMode::Fixed);

                treeView->header()->resizeSection(1, 100);
                treeView->header()->resizeSection(2, 100);

                requirementsLayout->addWidget(treeView);
            }

            const auto updateTreeViewVisibility = [treeViews, recommendedCheckBox]() -> void {
                treeViews.first()->setVisible(!recommendedCheckBox->isChecked());
                treeViews.last()->setVisible(recommendedCheckBox->isChecked());
            };

            updateTreeViewVisibility();

            connect(recommendedCheckBox, &QCheckBox::toggled, this, updateTreeViewVisibility);

            requirementsLayout->addWidget(recommendedCheckBox);

            layout->addLayout(requirementsLayout, 1);
            layout->addWidget(new QLabel("<p>Do you want to continue anyway?</p>"));

            auto dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Yes | QDialogButtonBox::Abort);

            connect(dialogButtonBox->button(QDialogButtonBox::StandardButton::Yes), &QPushButton::clicked, &projectIncompatibleWithSystemDialog, &QDialog::accept);
            connect(dialogButtonBox->button(QDialogButtonBox::StandardButton::Abort), &QPushButton::clicked, &projectIncompatibleWithSystemDialog, &QDialog::reject);

            layout->addWidget(dialogButtonBox);

            projectIncompatibleWithSystemDialog.setLayout(layout);

            if (projectIncompatibleWithSystemDialog.exec() == QDialog::Accepted) {
                projects().openProject(project->getUrl());
            }
        } else {
            openProject(project->getUrl(), targetDirectory, importDataOnly, loadWorkspace);
        }

        if (!project->getUrl().isLocalFile())
            project->setDownloaded();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to open ManiVault project", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to open ManiVault project due to an unhandled exception");
    }
}

void ProjectManager::importProject(QString filePath /*= ""*/)
{
    try
    {
#ifdef PROJECT_MANAGER_VERBOSE
        qDebug() << __FUNCTION__ << filePath;
#endif

        setState(State::ImportingProject);

        emit projectAboutToBeImported(filePath);
        {
            openProject(filePath, true, false);

            setState(State::Idle);
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

        if (hasActiveWorkflow())
            throw std::runtime_error("Another workflow is active");

        // State is already set in projectManager::publishProject(...)
        if (!isPublishingProject())
            setState(State::SavingProject);

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
                saveFileDialog.setDirectory(Application::current()->getSetting("Projects/WorkingDirectory", StandardPaths::getProjectsDirectory()).toString());

                auto fileDialogLayout   = dynamic_cast<QGridLayout*>(saveFileDialog.layout());
                auto rowCount           = fileDialogLayout->rowCount();

                fileDialogLayout->addWidget(_project->getCompressionAction().createLabelWidget(&saveFileDialog), rowCount + 2, 0);
                fileDialogLayout->addWidget(_project->getCompressionAction().createWidget(&saveFileDialog), rowCount + 2, 1, 1, 2);

                auto& titleAction = _project->getTitleAction();

                fileDialogLayout->addWidget(titleAction.createLabelWidget(nullptr), rowCount + 3, 0);

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

                fileDialogLayout->addLayout(titleLayout, rowCount + 3, 1, 1, 2);

                /*
                connect(&saveFileDialog, &QFileDialog::currentChanged, this, [this](const QString& filePath) -> void {
                    if (!QFileInfo(filePath).isFile())
                        return;

                    const auto projectMetaAction = Project::getProjectMetaActionFromProjectFilePath(filePath);

                    if (projectMetaAction.isNull())
                        return;

                    //_project->getCompressionAction().getEnabledAction().setChecked(projectMetaAction->getCompressionAction().getEnabledAction().isChecked());
                    //_project->getCompressionAction().getLevelAction().setValue(projectMetaAction->getCompressionAction().getLevelAction().getValue());
                });
                */

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

            auto workflow = std::make_unique<ProjectSaveWorkflow>(filePath);

            setTemporaryDirPath(TemporaryDirType::Save, workflow->getTemporaryDirPath());

            //connect(workflow.get(), &ProjectSaveWorkflow::finished, this, [this, filePath, workflowPtr = workflow.get()](bool success, const QString& error) {
            //    setState(State::Idle);

            //    if (success) {
            //        setState(State::Idle);
            //        emit projectOpened(*_project);
            //    }

            //    resetActiveWorkflow();
            //});

            setActiveWorkflow(std::move(workflow));
            getActiveWorkflow()->start();
        }
        emit projectSaved(*_project);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to save project", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to save project");
    }

    Application::current()->restoreOverrideCursor();
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
        if (hasActiveWorkflow())
            throw std::runtime_error("Another workflow is active");

        if (!hasProject())
            return;

        setState(State::PublishingProject);

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

            currentProject->getAllowedPluginsAction().addStrings(mv::plugins().getUsedPluginKinds());
            //currentProject->getAllowedPluginsAction().setLockedStrings(mv::plugins().getUsedPluginKinds());

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
                saveFileDialog.setDirectory(Application::current()->getSetting("Projects/WorkingDirectory", StandardPaths::getProjectsDirectory()).toString());

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
                    //compressionLayout->addWidget(currentProject->getCompressionAction().getLevelAction().createWidget(&saveFileDialog), 1);

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
                    settingsGroupAction.addAction(&currentProject->getProjectVersionAction());
                    //settingsGroupAction.addAction(&currentProject->getSplashScreenAction());
                    //settingsGroupAction.addAction(&currentProject->getOverrideApplicationStatusBarAction());
                    //settingsGroupAction.addAction(&currentProject->getStatusBarVisibleAction());
                    settingsGroupAction.addAction(&currentProject->getAllowedPluginsOnlyAction());
                    settingsGroupAction.addAction(&currentProject->getAllowedPluginsAction());
                    settingsGroupAction.addAction(&currentProject->getAllowProjectSwitchingAction());

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
                    //currentProject->getCompressionAction().getLevelAction().setValue(projectMetaAction->getCompressionAction().getLevelAction().getValue());
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

            Application::requestOverrideCursor(Qt::WaitCursor);

            auto& workspaceLockingAction = workspaces().getCurrentWorkspace()->getLockingAction();

            const auto cacheWorkspaceLocked = workspaceLockingAction.isLocked();

            workspaceLockingAction.setLocked(true);
            {
                auto& readOnlyAction            = getCurrentProject()->getReadOnlyAction();

                QSignalBlocker readOnlyActionSignalBlocker(&readOnlyAction);

                readOnlyAction.cacheState();
                readOnlyAction.setChecked(true);

                saveProject(filePath, passwordAction.getString());

                readOnlyAction.restoreState();
            }
            workspaceLockingAction.setLocked(cacheWorkspaceLocked);

            currentProject->getOverrideApplicationStatusBarAction().restoreState();
            currentProject->getProjectMetaAction().getApplicationVersionAction().setVersion(Application::current()->getVersion());

            unsetTemporaryDirPath(TemporaryDirType::Publish);

            Application::current()->restoreOverrideCursor();
        }
        emit projectPublished(*_project);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to publish ManiVault project", e);

        Application::current()->restoreOverrideCursor();
    }
    catch (...)
    {
        exceptionMessageBox("Unable to publish ManiVault project");

        Application::current()->restoreOverrideCursor();
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

const ProjectsListModel& ProjectManager::getProjectsListModel() const
{
    return _projectsListModel;
}

const ProjectsTreeModel& ProjectManager::getProjectsTreeModel() const
{
    return _projectsTreeModel;
}

QFuture<QString> ProjectManager::downloadProjectAsync(QUrl url, const QString& targetDirectory /*= ""*/, Task* task /*= nullptr*/)
{
    return FileDownloader::downloadToFileAsync(url, targetDirectory.isEmpty() ? getDownloadedProjectsDir().absolutePath() : targetDirectory, task ? task : &getProjectDownloadTask());
}

QFuture<bool> ProjectManager::isDownloadedProjectStaleAsync(QUrl url) const
{
    auto promise = std::make_shared<QPromise<bool>>();

    QFuture<bool> future = promise->future();

    QMetaObject::invokeMethod(qApp, [this, promise, url]() mutable {
        auto modifiedWatcher    = new QFutureWatcher<QDateTime>(const_cast<ProjectManager*>(this));
        auto sizeWatcher        = new QFutureWatcher<std::uint64_t>(const_cast<ProjectManager*>(this));
        auto finalNameWatcher   = new QFutureWatcher<QString>(const_cast<ProjectManager*>(this));

        auto modifiedFuture     = FileDownloader::getLastModifiedAsync(url);
        auto sizeFuture         = FileDownloader::getDownloadSizeAsync(url);
        auto finalNameFuture    = FileDownloader::getFinalFileNameAsync(url);

        modifiedWatcher->setFuture(modifiedFuture);
        sizeWatcher->setFuture(sizeFuture);
        finalNameWatcher->setFuture(finalNameFuture);

        auto checkAllFinished = [this, promise, url, modifiedWatcher, sizeWatcher, finalNameWatcher, modifiedFuture, sizeFuture, finalNameFuture]() mutable {
            if (!modifiedWatcher->isFinished() || !sizeWatcher->isFinished() || !finalNameWatcher->isFinished())
                return;

            try {
                if (modifiedFuture.resultCount() > 0 && sizeFuture.resultCount() > 0 && finalNameFuture.resultCount() > 0) {
                    const auto serverLastModified   = modifiedFuture.result();
                    const auto serverDownloadSize   = sizeFuture.result();
                    const auto fileName             = finalNameFuture.result();
                    const auto downloadedPath       = getDownloadedProjectsDir().filePath(fileName);
                    const auto localInfo            = QFileInfo(downloadedPath);
                    const auto localModified        = localInfo.lastModified();
                    const auto localSize            = static_cast<quint64>(localInfo.size());
                    const bool hasNewerTimestamp    = serverLastModified.isValid() && serverLastModified > localModified;
                    const bool sizeMismatch         = serverDownloadSize > 0 && serverDownloadSize != localSize;

                    promise->addResult(hasNewerTimestamp || sizeMismatch);
                }
                else {
                    if (modifiedFuture.resultCount() == 0)
                        throw BaseException("Failed to get last modified date and time headers from server");

                    if (sizeFuture.resultCount() == 0)
                        throw BaseException("Failed to get size headers from server");

                    if (finalNameFuture.resultCount() == 0)
                        throw BaseException("Failed to get file name info from server");
                }
            }
            catch (const QException& e) {
                promise->setException(std::make_exception_ptr(BaseException(QString("Failed to compare download state: %1").arg(e.what()))));
            }
            catch (...) {
                promise->setException(std::make_exception_ptr(BaseException("Unknown failure in stale check")));
            }

            promise->finish();

            modifiedWatcher->deleteLater();
            sizeWatcher->deleteLater();
            finalNameWatcher->deleteLater();
        };

        connect(modifiedWatcher, &QFutureWatcherBase::finished, checkAllFinished);
        connect(sizeWatcher, &QFutureWatcherBase::finished, checkAllFinished);
        connect(finalNameWatcher, &QFutureWatcherBase::finished, checkAllFinished);
    });

    return future;
}

QDir ProjectManager::getDownloadedProjectsDir() const
{
    QDir downloadsDir(StandardPaths::getDownloadsDirectory());

    const auto fullPath = downloadsDir.filePath("Projects");

    QDir resultDir(fullPath);

    if (!resultDir.exists()) {
        [[maybe_unused]] const auto madePath = QDir().mkpath(fullPath);
    }

    return resultDir;
}

AbstractSerializationPlanExecutor* ProjectManager::getSerializationPlanExecutor()
{
    return &_serializationPlanExecutor;
}

AbstractWorkflow* ProjectManager::getActiveWorkflow()
{
	return _activeWorkflow.get();
}

bool ProjectManager::hasActiveWorkflow() const
{
	return _activeWorkflow != nullptr;
}

void ProjectManager::setActiveWorkflow(util::UniqueAbstractWorkflow activeWorkflow)
{
	if (_activeWorkflow)
		throw std::runtime_error("Cannot set active workflow, another workflow is already active");

	_activeWorkflow = std::move(activeWorkflow);
}

void ProjectManager::resetActiveWorkflow()
{
    _activeWorkflow.reset();
}

QString ProjectManager::chooseProjectFileViaDialog()
{
	FileOpenDialog fileOpenDialog;

	fileOpenDialog.setWindowTitle("Open ManiVault Project");
	fileOpenDialog.setNameFilters({ "ManiVault project files (*.mv)" });
	fileOpenDialog.setDefaultSuffix(".mv");
	fileOpenDialog.setDirectory(
		Application::current()->getSetting(
			"Projects/WorkingDirectory",
			StandardPaths::getProjectsDirectory()).toString());
	fileOpenDialog.setOption(QFileDialog::DontUseNativeDialog, true);

	ToggleAction disableReadOnlyAction(this, "Allow edit of published project");
	StringAction titleAction(this, "Title");
	StringAction descriptionAction(this, "Description");
	StringAction tagsAction(this, "Tags");
	StringAction commentsAction(this, "Comments");
	StringAction contributorsAction(this, "Contributors");

	titleAction.setEnabled(false);
	descriptionAction.setEnabled(false);
	tagsAction.setEnabled(false);
	commentsAction.setEnabled(false);
	contributorsAction.setEnabled(false);
	disableReadOnlyAction.setEnabled(false);

	auto*     fileDialogLayout = dynamic_cast<QGridLayout*>(fileOpenDialog.layout());
	const int rowCount         = fileDialogLayout->rowCount();

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

	connect(&fileOpenDialog, &QFileDialog::currentChanged, this, [&](const QString& filePath) {
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
		return {};

	if (fileOpenDialog.selectedFiles().count() != 1)
		throw std::runtime_error("Only one file may be selected");

	const QString filePath = fileOpenDialog.selectedFiles().first();

	Application::current()->setSetting(
		"Projects/WorkingDirectory",
		QFileInfo(filePath).absolutePath());

	return filePath;
}

QMenu& ProjectManager::getNewProjectMenu()
{
    return _newProjectMenu;
}

QMenu& ProjectManager::getImportDataMenu()
{
    return _importDataMenu;
}

ProjectMetaAction* ProjectManager::getProjectMetaAction(const QString& projectFilePath)
{
    try {
        const QString metaJsonFilePath("meta.json");

        QFileInfo extractFileInfo(Application::current()->getTemporaryDir().path(), metaJsonFilePath);

        Archiver archiver;

        QString extractedMetaJsonFilePath = "";

        archiver.extractSingleFile(projectFilePath, metaJsonFilePath, extractFileInfo.absoluteFilePath());

        extractedMetaJsonFilePath = extractFileInfo.absoluteFilePath();

        if (!QFileInfo(extractedMetaJsonFilePath).exists())
            throw std::runtime_error("Unable to extract meta.json");

        return new ProjectMetaAction(extractedMetaJsonFilePath, Application::current());
    }
    catch (std::exception& e)
    {
        qDebug() << "No meta data available, please re-save the project to solve the problem" << e.what();
    }
    catch (...)
    {
        qDebug() << "No meta data available due to an unhandled problem, please re-save the project to solve the problem";
    }

    return {};
}

bool ProjectManager::isMvFileName(const QString& name)
{
    return QFileInfo(name).suffix().compare(QStringLiteral("mv"), Qt::CaseInsensitive) == 0;
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
