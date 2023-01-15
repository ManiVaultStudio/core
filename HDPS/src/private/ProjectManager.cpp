#include "ProjectManager.h"
#include "Archiver.h"
#include "PluginManagerDialog.h"
#include "ProjectSettingsDialog.h"
#include "NewProjectDialog.h"

#include <Application.h>
#include <CoreInterface.h>

#include <util/Exception.h>
#include <util/Serialization.h>

#include <widgets/TaskProgressDialog.h>

#include <QFileDialog>
#include <QStandardPaths>
#include <QGridLayout>

#ifdef _DEBUG
    #define PROJECT_MANAGER_VERBOSE
#endif

using namespace hdps;
using namespace hdps::util;
using namespace hdps::gui;

// Definition needed for pre C++17 gcc and clang
#if (__cplusplus < 201703L)
    constexpr bool ProjectManager::DEFAULT_ENABLE_COMPRESSION;
    constexpr std::uint32_t ProjectManager::DEFAULT_COMPRESSION_LEVEL;
#endif

ProjectManager::ProjectManager(QObject* parent /*= nullptr*/) :
    _project(),
    _newBlankProjectAction(nullptr, "Blank"),
    _newProjectFromWorkspaceAction(nullptr, "From Workspace..."),
    _openProjectAction(nullptr, "Open Project"),
    _importProjectAction(nullptr, "Import Project"),
    _saveProjectAction(nullptr, "Save Project"),
    _saveProjectAsAction(nullptr, "Save Project As..."),
    _editProjectSettingsAction(nullptr, "Project Settings..."),
    _recentProjectsAction(nullptr),
    _importDataMenu(),
    _publishAction(nullptr, "Publish"),
    _pluginManagerAction(nullptr, "Plugin Manager"),
    _showStartPageAction(nullptr, "Start Page...", true, true)
{
    _newBlankProjectAction.setShortcut(QKeySequence("Ctrl+B"));
    _newBlankProjectAction.setShortcutContext(Qt::ApplicationShortcut);
    _newBlankProjectAction.setIcon(Application::getIconFont("FontAwesome").getIcon("file"));
    _newBlankProjectAction.setToolTip("Create project without view plugins and data");

    _newProjectFromWorkspaceAction.setShortcut(QKeySequence("Ctrl+N"));
    _newProjectFromWorkspaceAction.setShortcutContext(Qt::ApplicationShortcut);
    _newProjectFromWorkspaceAction.setIcon(workspaces().getIcon());
    _newProjectFromWorkspaceAction.setToolTip("Create new project with workspace");

    _openProjectAction.setShortcut(QKeySequence("Ctrl+O"));
    _openProjectAction.setShortcutContext(Qt::ApplicationShortcut);
    _openProjectAction.setIcon(Application::getIconFont("FontAwesome").getIcon("folder-open"));
    _openProjectAction.setToolTip("Open project from disk");

    _importProjectAction.setShortcut(QKeySequence("Ctrl+I"));
    _importProjectAction.setShortcutContext(Qt::ApplicationShortcut);
    _importProjectAction.setIcon(Application::getIconFont("FontAwesome").getIcon("file-import"));
    _importProjectAction.setToolTip("Import project from disk");

    _saveProjectAction.setShortcut(QKeySequence("Ctrl+S"));
    _saveProjectAction.setShortcutContext(Qt::ApplicationShortcut);
    _saveProjectAction.setIcon(Application::getIconFont("FontAwesome").getIcon("save"));
    _saveProjectAction.setToolTip("Save project to disk");

    _saveProjectAsAction.setShortcut(QKeySequence("Ctrl+Shift+S"));
    _saveProjectAsAction.setShortcutContext(Qt::ApplicationShortcut);
    _saveProjectAsAction.setIcon(Application::getIconFont("FontAwesome").getIcon("save"));
    _saveProjectAsAction.setToolTip("Save project to disk in a chosen location");

    _editProjectSettingsAction.setShortcut(QKeySequence("Ctrl+P"));
    _editProjectSettingsAction.setShortcutContext(Qt::ApplicationShortcut);
    _editProjectSettingsAction.setIcon(Application::getIconFont("FontAwesome").getIcon("cog"));
    _editProjectSettingsAction.setConnectionPermissionsToNone();

    _newProjectMenu.setIcon(Application::getIconFont("FontAwesome").getIcon("file"));
    _newProjectMenu.setTitle("New Project");
    _newProjectMenu.setToolTip("Create new project");
    _newProjectMenu.addAction(&_newBlankProjectAction);
    _newProjectMenu.addAction(&_newProjectFromWorkspaceAction);

    _importDataMenu.setIcon(Application::getIconFont("FontAwesome").getIcon("file-import"));
    _importDataMenu.setTitle("Import data...");
    _importDataMenu.setToolTip("Import data into HDPS");

    _publishAction.setShortcut(QKeySequence("Ctrl+P"));
    _publishAction.setShortcutContext(Qt::ApplicationShortcut);
    _publishAction.setIcon(Application::getIconFont("FontAwesome").getIcon("share"));
    _publishAction.setToolTip("Publish the HDPS application");

    _pluginManagerAction.setShortcut(QKeySequence("Ctrl+M"));
    _pluginManagerAction.setShortcutContext(Qt::ApplicationShortcut);
    _pluginManagerAction.setIcon(Application::getIconFont("FontAwesome").getIcon("plug"));
    _pluginManagerAction.setToolTip("View loaded plugins");

    _showStartPageAction.setShortcut(QKeySequence("Alt+W"));
    _showStartPageAction.setShortcutContext(Qt::ApplicationShortcut);
    _showStartPageAction.setIcon(Application::getIconFont("FontAwesome").getIcon("door-open"));
    _showStartPageAction.setToolTip("Show the HDPS start page");

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
        NewProjectDialog newProjectDialog;
        newProjectDialog.exec();
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

        saveProject(_project->getFilePath());
    });

    connect(&_saveProjectAsAction, &QAction::triggered, [this]() -> void {
        if (_project.isNull())
            return;

        saveProjectAs();
    });

    connect(&_editProjectSettingsAction, &TriggerAction::triggered, this, []() -> void {
        ProjectSettingsDialog projectSettingsDialog;
        projectSettingsDialog.exec();
    });

    connect(&_importDataMenu, &QMenu::aboutToShow, this, [this]() -> void {
        _importDataMenu.clear();

        for (auto pluginTriggerAction : plugins().getPluginTriggerActions(plugin::Type::LOADER))
            _importDataMenu.addAction(pluginTriggerAction);
    });

    connect(&_pluginManagerAction, &TriggerAction::triggered, this, [this]() -> void {
        PluginManagerDialog::create();
    });

    const auto updateActionsReadOnly = [this]() -> void {
        _saveProjectAction.setEnabled(hasProject());
        _saveProjectAsAction.setEnabled(hasProject());
        _saveProjectAsAction.setEnabled(hasProject() && !_project->getFilePath().isEmpty());
        _editProjectSettingsAction.setEnabled(hasProject());
        _importProjectAction.setEnabled(hasProject());
    };

    connect(this, &ProjectManager::projectCreated, this, updateActionsReadOnly);
    connect(this, &ProjectManager::projectDestroyed, this, updateActionsReadOnly);
    connect(this, &ProjectManager::projectLoaded, this, updateActionsReadOnly);
    connect(this, &ProjectManager::projectSaved, this, updateActionsReadOnly);

    updateActionsReadOnly();

    _recentProjectsAction.initialize("Manager/Project/Recent", "Project", "Ctrl", Application::getIconFont("FontAwesome").getIcon("file"));

    connect(&_recentProjectsAction, &RecentFilesAction::triggered, this, [this](const QString& filePath) -> void {
        openProject(filePath);
    });
}

void ProjectManager::initalize()
{
#ifdef PROJECT_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif
}

void ProjectManager::reset()
{
#ifdef PROJECT_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    beginReset();
    {
        auto core = Application::core();

        core->getActionsManager().reset();
        core->getPluginManager().reset();
        core->getDataManager().reset();
        core->getDataHierarchyManager().reset();
        core->getWorkspaceManager().reset();
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

void ProjectManager::newBlankProject()
{
    try
    {
#ifdef PROJECT_MANAGER_VERBOSE
        qDebug() << __FUNCTION__;
#endif
        createProject();
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

        emit projectAboutToBeLoaded(*(_project.get()));
        {
            if (QFileInfo(filePath).isDir())
                throw std::runtime_error("Project file path may not be a directory");

            const auto loadedDatasets = Application::core()->requestAllDataSets();

            QTemporaryDir temporaryDirectory;

            const auto temporaryDirectoryPath = temporaryDirectory.path();

            Application::setSerializationTemporaryDirectory(temporaryDirectoryPath);
            Application::setSerializationAborted(false);

            if (filePath.isEmpty()) {
                QFileDialog fileDialog;

                fileDialog.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("folder-open"));
                fileDialog.setWindowTitle("Open Project");
                fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
                fileDialog.setFileMode(QFileDialog::ExistingFile);
                fileDialog.setNameFilters({ "HDPS project files (*.hdps)" });
                fileDialog.setDefaultSuffix(".hdps");
                fileDialog.setDirectory(Application::current()->getSetting("Projects/WorkingDirectory", QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)).toString());
                fileDialog.setOption(QFileDialog::DontUseNativeDialog, true);

                StringAction titleAction(this, "Title");
                StringAction descriptionAction(this, "Description");
                StringAction tagsAction(this, "Tags");
                StringAction commentsAction(this, "Comments");

                titleAction.setEnabled(false);
                descriptionAction.setEnabled(false);
                tagsAction.setEnabled(false);
                commentsAction.setEnabled(false);

                titleAction.setConnectionPermissionsToNone();
                descriptionAction.setConnectionPermissionsToNone();
                tagsAction.setConnectionPermissionsToNone();
                commentsAction.setConnectionPermissionsToNone();

                auto fileDialogLayout   = dynamic_cast<QGridLayout*>(fileDialog.layout());
                auto rowCount           = fileDialogLayout->rowCount();

                fileDialogLayout->addWidget(titleAction.createLabelWidget(&fileDialog), rowCount, 0);
                fileDialogLayout->addWidget(titleAction.createWidget(&fileDialog), rowCount, 1, 1, 2);

                fileDialogLayout->addWidget(descriptionAction.createLabelWidget(&fileDialog), rowCount + 1, 0);
                fileDialogLayout->addWidget(descriptionAction.createWidget(&fileDialog), rowCount + 1, 1, 1, 2);

                fileDialogLayout->addWidget(tagsAction.createLabelWidget(&fileDialog), rowCount + 2, 0);
                fileDialogLayout->addWidget(tagsAction.createWidget(&fileDialog), rowCount + 2, 1, 1, 2);

                fileDialogLayout->addWidget(commentsAction.createLabelWidget(&fileDialog), rowCount + 3, 0);
                fileDialogLayout->addWidget(commentsAction.createWidget(&fileDialog), rowCount + 3, 1, 1, 2);

                connect(&fileDialog, &QFileDialog::currentChanged, this, [&](const QString& filePath) -> void {
                    if (!QFileInfo(filePath).isFile())
                        return;

                    QTemporaryDir temporaryDir;

                    Project project(extractProjectFileFromHdpsFile(filePath, temporaryDir));

                    titleAction.setString(project.getTitleAction().getString());
                    descriptionAction.setString(project.getDescriptionAction().getString());
                    tagsAction.setString(project.getTagsAction().getStrings().join(", "));
                    commentsAction.setString(project.getCommentsAction().getString());
                });

                if (fileDialog.exec() == 0)
                    return;

                if (fileDialog.selectedFiles().count() != 1)
                    throw std::runtime_error("Only one file may be selected");

                filePath = fileDialog.selectedFiles().first();

                Application::current()->setSetting("Projects/WorkingDirectory", QFileInfo(filePath).absolutePath());
            }

            if (!importDataOnly)
                newProject();

            qDebug().noquote() << "Open HDPS project from" << filePath;

            Archiver archiver;

            QStringList tasks = archiver.getTaskNamesForDecompression(filePath) << "Import data model" << "Load workspace";

            TaskProgressDialog taskProgressDialog(nullptr, tasks, "Open HDPS project from " + filePath, Application::getIconFont("FontAwesome").getIcon("folder-open"));

            connect(&taskProgressDialog, &TaskProgressDialog::canceled, this, [this]() -> void {
                Application::setSerializationAborted(true);

                throw std::runtime_error("Canceled before project was loaded");
            });

            connect(&Application::core()->getDataHierarchyManager(), &AbstractDataHierarchyManager::itemLoading, this, [&taskProgressDialog](DataHierarchyItem& loadingItem) {
                //taskProgressDialog.setCurrentTask("Importing dataset: " + loadingItem.getFullPathName());
            });

            connect(&archiver, &Archiver::taskStarted, &taskProgressDialog, &TaskProgressDialog::setCurrentTask);
            connect(&archiver, &Archiver::taskFinished, &taskProgressDialog, &TaskProgressDialog::setTaskFinished);

            archiver.decompress(filePath, temporaryDirectoryPath);

            taskProgressDialog.setCurrentTask("Import data model");
            {
                Application::core()->getProjectManager().fromJsonFile(QFileInfo(temporaryDirectoryPath, "project.json").absoluteFilePath());
            }
            taskProgressDialog.setTaskFinished("Import data model");

            if (loadWorkspace) {
                taskProgressDialog.setCurrentTask("Load workspace");
                {
                    const QFileInfo workspaceFileInfo(temporaryDirectoryPath, "workspace.hws");

                    if (workspaceFileInfo.exists())
                        Application::core()->getWorkspaceManager().loadWorkspace(workspaceFileInfo.absoluteFilePath(), false);
                }
                taskProgressDialog.setTaskFinished("Load workspace");
            }

            _recentProjectsAction.addRecentFilePath(filePath);

            _project->setFilePath(filePath);

            qDebug().noquote() << filePath << "loaded successfully";
        }
        emit projectLoaded(*(_project.get()));
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to load HDPS project", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to load HDPS project");
    }
}

void ProjectManager::importProject(QString filePath /*= ""*/)
{
    emit projectAboutToBeImported(filePath);
    {
        openProject(filePath, true, false);
    }
    emit projectImported(filePath);
}

void ProjectManager::saveProject(QString filePath /*= ""*/)
{
    try
    {
#ifdef PROJECT_MANAGER_VERBOSE
        qDebug() << __FUNCTION__ << filePath;
#endif

        emit projectAboutToBeSaved(*(_project.get()));
        {
            if (QFileInfo(filePath).isDir())
                throw std::runtime_error("Project file path may not be a directory");

            QTemporaryDir temporaryDirectory;

            const auto temporaryDirectoryPath = temporaryDirectory.path();

            const auto getSettingsPrefix = [](const QString& filePath) -> QString {
                return "Projects/" + filePath + "/";
            };

            auto currentProject = getCurrentProject();

            if (filePath.isEmpty()) {

                QFileDialog fileDialog;

                fileDialog.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("save"));
                fileDialog.setWindowTitle("Save Project");
                fileDialog.setAcceptMode(QFileDialog::AcceptSave);
                fileDialog.setNameFilters({ "HDPS project files (*.hdps)" });
                fileDialog.setDefaultSuffix(".hdps");
                fileDialog.setOption(QFileDialog::DontUseNativeDialog, true);
                fileDialog.setDirectory(Application::current()->getSetting("Projects/WorkingDirectory", QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)).toString());

                auto fileDialogLayout   = dynamic_cast<QGridLayout*>(fileDialog.layout());
                auto rowCount           = fileDialogLayout->rowCount();

                QCheckBox   passwordProtectedCheckBox("Password protected");
                QLineEdit   passwordLineEdit;

                passwordProtectedCheckBox.setChecked(false);
                passwordLineEdit.setPlaceholderText("Enter encryption password...");

                auto compressionLayout = new QHBoxLayout();

                compressionLayout->addWidget(currentProject->getCompressionEnabledAction().createWidget(&fileDialog));
                compressionLayout->addWidget(currentProject->getCompressionLevelAction().createWidget(&fileDialog), 1);
                
                fileDialogLayout->addLayout(compressionLayout, rowCount, 1, 1, 2);
                
                //fileDialogLayout->addWidget(&passwordProtectedCheckBox, ++rowCount, 0);
                //fileDialogLayout->addWidget(&passwordLineEdit, rowCount, 1);

                auto& titleAction = currentProject->getTitleAction();

                fileDialogLayout->addWidget(titleAction.createLabelWidget(nullptr), rowCount + 2, 0);

                GroupAction settingsGroupAction(this);

                settingsGroupAction.setIcon(Application::getIconFont("FontAwesome").getIcon("cog"));
                settingsGroupAction.setToolTip("Edit project settings");
                settingsGroupAction.setPopupSizeHint(QSize(420, 320));
                settingsGroupAction.setLabelSizingType(GroupAction::LabelSizingType::Auto);

                settingsGroupAction << currentProject->getTitleAction();
                settingsGroupAction << currentProject->getDescriptionAction();
                settingsGroupAction << currentProject->getTagsAction();
                settingsGroupAction << currentProject->getCommentsAction();

                auto titleLayout = new QHBoxLayout();

                titleLayout->addWidget(titleAction.createWidget(&fileDialog));
                titleLayout->addWidget(settingsGroupAction.createCollapsedWidget(&fileDialog));

                fileDialogLayout->addLayout(titleLayout, rowCount + 2, 1, 1, 2);

                //const auto updatePassword = [&]() -> void {
                //    passwordLineEdit.setEnabled(passwordProtectedCheckBox.isChecked());
                //};

                //connect(&passwordProtectedCheckBox, &QCheckBox::toggled, this, updatePassword);

                //updatePassword();

                connect(&fileDialog, &QFileDialog::currentChanged, this, [this, getSettingsPrefix, currentProject](const QString& path) -> void {
                    Project project(path);

                    currentProject->getCompressionEnabledAction().setChecked(project.getCompressionEnabledAction().isChecked());
                    currentProject->getCompressionLevelAction().setValue(project.getCompressionLevelAction().getValue());
                });

                fileDialog.exec();

                if (fileDialog.selectedFiles().count() != 1)
                    throw std::runtime_error("Only one file may be selected");

                filePath = fileDialog.selectedFiles().first();

                Application::current()->setSetting("Projects/WorkingDirectory", QFileInfo(filePath).absolutePath());
            }
            
            if (filePath.isEmpty() || QFileInfo(filePath).isDir())
                return;

            if (currentProject->getCompressionEnabledAction().isChecked())
                qDebug().noquote() << "Saving HDPS project to" << filePath << "with compression level" << currentProject->getCompressionLevelAction().getValue();
            else
                qDebug().noquote() << "Saving HDPS project to" << filePath << "without compression";

            Archiver archiver;

            QStringList tasks;

            tasks << "Export data model" << "Temporary task";

            TaskProgressDialog taskProgressDialog(nullptr, tasks, "Saving HDPS project to " + filePath, Application::current()->getIconFont("FontAwesome").getIcon("save"));

            taskProgressDialog.setCurrentTask("Export data model");

            connect(&taskProgressDialog, &TaskProgressDialog::canceled, this, [this]() -> void {
                Application::setSerializationAborted(true);

                throw std::runtime_error("Canceled before project was saved");
            });

            QFileInfo jsonFileInfo(temporaryDirectoryPath, "project.json");

            Application::setSerializationTemporaryDirectory(temporaryDirectoryPath);
            Application::setSerializationAborted(false);

            connect(&Application::core()->getDataHierarchyManager(), &AbstractDataHierarchyManager::itemSaving, this, [&taskProgressDialog](DataHierarchyItem& savingItem) {
                taskProgressDialog.setCurrentTask("Exporting dataset: " + savingItem.getFullPathName());
            });

            Application::core()->getProjectManager().toJsonFile(jsonFileInfo.absoluteFilePath());

            taskProgressDialog.setTaskFinished("Export data model");
            taskProgressDialog.addTasks(archiver.getTaskNamesForDirectoryCompression(temporaryDirectoryPath));
            taskProgressDialog.setTaskFinished("Temporary task");

            connect(&archiver, &Archiver::taskStarted, &taskProgressDialog, &TaskProgressDialog::setCurrentTask);
            connect(&archiver, &Archiver::taskFinished, &taskProgressDialog, &TaskProgressDialog::setTaskFinished);

            QFileInfo workspaceFileInfo(temporaryDirectoryPath, "workspace.hws");

            Application::core()->getWorkspaceManager().saveWorkspace(workspaceFileInfo.absoluteFilePath(), false);

            archiver.compressDirectory(temporaryDirectoryPath, filePath, true, currentProject->getCompressionEnabledAction().isChecked() ? currentProject->getCompressionLevelAction().getValue() : 0, "");

            _recentProjectsAction.addRecentFilePath(filePath);

            _project->setFilePath(filePath);

            qDebug().noquote() << filePath << "saved successfully";
        }
        emit projectSaved(*(_project.get()));
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to save project", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to save project");
    }
}

void ProjectManager::saveProjectAs()
{
    saveProject("");
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
        reset();

        _project.reset(new Project());
    }
    emit projectCreated(*(_project.get()));

    _showStartPageAction.setChecked(false);

    Application::core()->getWorkspaceManager().reset();
}

bool ProjectManager::hasProject() const
{
    return getCurrentProject() != nullptr;
}

const hdps::Project* ProjectManager::getCurrentProject() const
{
    return _project.get();
}

hdps::Project* ProjectManager::getCurrentProject()
{
    return _project.get();
}

QString ProjectManager::extractProjectFileFromHdpsFile(const QString& hdpsFilePath, QTemporaryDir& temporaryDir)
{
    const auto temporaryDirectoryPath = temporaryDir.path();

    const QString projectFile("project.json");

    QFileInfo projectFileInfo(temporaryDirectoryPath, projectFile);

    Archiver archiver;

    archiver.extractSingleFile(hdpsFilePath, projectFile, projectFileInfo.absoluteFilePath());

    return projectFileInfo.absoluteFilePath();
}

void ProjectManager::fromVariantMap(const QVariantMap& variantMap)
{
    createProject();

    _project->fromVariantMap(variantMap);
}

QVariantMap ProjectManager::toVariantMap() const
{
    if (hasProject())
        return _project->toVariantMap();

    return QVariantMap();
}
