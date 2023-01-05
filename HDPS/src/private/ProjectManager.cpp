#include "ProjectManager.h"
#include "Archiver.h"
#include "PluginManagerDialog.h"
#include "ProjectSettingsDialog.h"
#include "NewProjectDialog.h"

#include <Application.h>

#include <util/Exception.h>
#include <util/Serialization.h>

#include <widgets/TaskProgressDialog.h>

#include <QFileDialog>
#include <QTemporaryDir>
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
    _newProjectAction(nullptr, "New Project..."),
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
    _newProjectAction.setShortcut(QKeySequence("Ctrl+N"));
    _newProjectAction.setShortcutContext(Qt::ApplicationShortcut);
    _newProjectAction.setIcon(Application::getIconFont("FontAwesome").getIcon("file"));
    _newProjectAction.setToolTip("Open project from disk");

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

    mainWindow->addAction(&_newProjectAction);
    mainWindow->addAction(&_openProjectAction);
    mainWindow->addAction(&_importProjectAction);
    mainWindow->addAction(&_saveProjectAction);
    mainWindow->addAction(&_showStartPageAction);

    connect(&_newProjectAction, &QAction::triggered, this, [this]() -> void {
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

        for (auto pluginTriggerAction : Application::core()->getPluginManager().getPluginTriggerActions(plugin::Type::LOADER))
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

        Application::core()->getWorkspaceManager().loadWorkspace(workspaceFilePath);
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
                fileDialog.setWindowTitle("Load Project");
                fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
                fileDialog.setFileMode(QFileDialog::ExistingFile);
                fileDialog.setNameFilters({ "HDPS project files (*.hdps)" });
                fileDialog.setDefaultSuffix(".hdps");
                fileDialog.setDirectory(Application::current()->getSetting("Projects/WorkingDirectory", QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)).toString());
                fileDialog.setOption(QFileDialog::DontUseNativeDialog, true);

                StringAction descriptionAction(this, "Description");
                StringAction tagsAction(this, "Tags");
                StringAction commentsAction(this, "Comments");

                descriptionAction.setEnabled(false);
                tagsAction.setEnabled(false);
                commentsAction.setEnabled(false);

                descriptionAction.setConnectionPermissionsToNone();
                tagsAction.setConnectionPermissionsToNone();
                commentsAction.setConnectionPermissionsToNone();

                auto fileDialogLayout   = dynamic_cast<QGridLayout*>(fileDialog.layout());
                auto rowCount           = fileDialogLayout->rowCount();

                fileDialogLayout->addWidget(descriptionAction.createLabelWidget(&fileDialog), rowCount, 0);
                fileDialogLayout->addWidget(descriptionAction.createWidget(&fileDialog), rowCount, 1, 1, 2);

                fileDialogLayout->addWidget(tagsAction.createLabelWidget(&fileDialog), rowCount + 1, 0);
                fileDialogLayout->addWidget(tagsAction.createWidget(&fileDialog), rowCount + 1, 1, 1, 2);

                fileDialogLayout->addWidget(commentsAction.createLabelWidget(&fileDialog), rowCount + 2, 0);
                fileDialogLayout->addWidget(commentsAction.createWidget(&fileDialog), rowCount + 2, 1, 1, 2);

                connect(&fileDialog, &QFileDialog::currentChanged, this, [&](const QString& filePath) -> void {
                    if (!QFileInfo(filePath).isFile())
                        return;

                    QTemporaryDir temporaryDirectory;

                    const auto temporaryDirectoryPath = temporaryDirectory.path();

                    Application::setSerializationTemporaryDirectory(temporaryDirectoryPath);
                    Application::setSerializationAborted(false);

                    Archiver archiver;

                    const QString projectFile("project.json");

                    QFileInfo projectFileInfo(temporaryDirectoryPath, projectFile);

                    archiver.extractSingleFile(filePath, projectFile, projectFileInfo.absoluteFilePath());

                    Project project(projectFileInfo.absoluteFilePath());

                    descriptionAction.setString(project.getDescriptionAction().getString());
                    tagsAction.setString(project.getTagsAction().getStrings().join(", "));
                    commentsAction.setString(project.getDescriptionAction().getString());
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

            qDebug().noquote() << "Loading HDPS project from" << filePath;

            Archiver archiver;

            QStringList tasks = archiver.getTaskNamesForDecompression(filePath) << "Import data model";

            TaskProgressDialog taskProgressDialog(nullptr, tasks, "Loading HDPS project from " + filePath, Application::getIconFont("FontAwesome").getIcon("folder-open"));

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

            QFileInfo inputJsonFileInfo(temporaryDirectoryPath, "project.json");

            Application::core()->getProjectManager().fromJsonFile(inputJsonFileInfo.absoluteFilePath());

            if (loadWorkspace) {
                const QFileInfo workspaceFileInfo(temporaryDirectoryPath, "workspace.hws");

                if (workspaceFileInfo.exists())
                    Application::core()->getWorkspaceManager().loadWorkspace(workspaceFileInfo.absoluteFilePath(), false);
            }
                

            taskProgressDialog.setTaskFinished("Import data model");

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

            bool            enableCompression = DEFAULT_ENABLE_COMPRESSION;
            std::uint32_t   compressionLevel = DEFAULT_COMPRESSION_LEVEL;

            if (filePath.isEmpty()) {

                QFileDialog fileDialog;

                fileDialog.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("save"));
                fileDialog.setWindowTitle("Save Project");
                fileDialog.setAcceptMode(QFileDialog::AcceptSave);
                fileDialog.setNameFilters({ "HDPS project files (*.hdps)" });
                fileDialog.setDefaultSuffix(".hdps");
                fileDialog.setOption(QFileDialog::DontUseNativeDialog, true);
                fileDialog.setDirectory(Application::current()->getSetting("Projects/WorkingDirectory", QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)).toString());

                auto fileDialogLayout = dynamic_cast<QGridLayout*>(fileDialog.layout());
                auto rowCount = fileDialogLayout->rowCount();

                QCheckBox   enableCompressionCheckBox("Compression:");
                QSpinBox    compressionLevelSpinBox;
                QCheckBox   passwordProtectedCheckBox("Password protected");
                QLineEdit   passwordLineEdit;

                enableCompressionCheckBox.setChecked(enableCompression);
                compressionLevelSpinBox.setPrefix("Level: ");
                compressionLevelSpinBox.setMinimum(1);
                compressionLevelSpinBox.setMaximum(9);
                compressionLevelSpinBox.setValue(compressionLevel);
                passwordProtectedCheckBox.setChecked(false);
                passwordLineEdit.setPlaceholderText("Enter encryption password...");

                fileDialogLayout->addWidget(&enableCompressionCheckBox, rowCount, 0);
                fileDialogLayout->addWidget(&compressionLevelSpinBox, rowCount, 1);
                //fileDialogLayout->addWidget(&passwordProtectedCheckBox, ++rowCount, 0);
                //fileDialogLayout->addWidget(&passwordLineEdit, rowCount, 1);

                const auto updateCompressionLevel = [&]() -> void {
                    compressionLevelSpinBox.setEnabled(enableCompressionCheckBox.isChecked());
                };

                const auto updatePassword = [&]() -> void {
                    passwordLineEdit.setEnabled(passwordProtectedCheckBox.isChecked());
                };

                connect(&enableCompressionCheckBox, &QCheckBox::toggled, this, updateCompressionLevel);
                connect(&passwordProtectedCheckBox, &QCheckBox::toggled, this, updatePassword);

                updateCompressionLevel();
                updatePassword();

                connect(&fileDialog, &QFileDialog::currentChanged, this, [this, getSettingsPrefix, enableCompression, compressionLevel, &enableCompressionCheckBox, &compressionLevelSpinBox](const QString& path) -> void {
                    enableCompressionCheckBox.setChecked(Application::current()->getSetting(getSettingsPrefix(path) + "EnableCompression", enableCompression).toBool());
                    compressionLevelSpinBox.setValue(Application::current()->getSetting(getSettingsPrefix(path) + "CompressionLevel", compressionLevel).toInt());
                    });

                fileDialog.exec();

                if (fileDialog.selectedFiles().count() != 1)
                    throw std::runtime_error("Only one file may be selected");

                filePath = fileDialog.selectedFiles().first();

                enableCompression = enableCompressionCheckBox.isChecked();
                compressionLevel = compressionLevelSpinBox.value();

                Application::current()->setSetting("Projects/WorkingDirectory", QFileInfo(filePath).absolutePath());
            }
            else
            {
                enableCompression = Application::current()->getSetting(getSettingsPrefix(filePath) + "EnableCompression", enableCompression).toBool();
                compressionLevel = Application::current()->getSetting(getSettingsPrefix(filePath) + "CompressionLevel", compressionLevel).toInt();
            }

            if (filePath.isEmpty() || QFileInfo(filePath).isDir())
                return;

            if (enableCompression)
                qDebug().noquote() << "Saving HDPS project to" << filePath << "with compression level" << compressionLevel;
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

            Application::core()->getWorkspaceManager().saveWorkspace(workspaceFileInfo.absoluteFilePath());

            archiver.compressDirectory(temporaryDirectoryPath, filePath, true, enableCompression ? compressionLevel : 0, "");

            _recentProjectsAction.addRecentFilePath(filePath);

            Application::current()->setSetting(getSettingsPrefix(filePath) + "EnableCompression", enableCompression);
            Application::current()->setSetting(getSettingsPrefix(filePath) + "CompressionLevel", compressionLevel);

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

QMenu* ProjectManager::getImportDataMenu()
{
    return &_importDataMenu;
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
    return getProject() != nullptr;
}

const hdps::Project* ProjectManager::getProject() const
{
    return _project.get();
}

hdps::Project* ProjectManager::getProject()
{
    return _project.get();
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
