#include "ProjectManager.h"
#include "Archiver.h"
#include "PluginManagerDialog.h"

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
    _newProjectAction(nullptr, "New Project"),
    _openProjectAction(nullptr, "Open Project"),
    _saveProjectAction(nullptr, "Save Project"),
    _saveProjectAsAction(nullptr, "Save Project As..."),
    _recentProjectsAction(nullptr),
    _importDataMenu(),
    _publishAction(nullptr, "Publish"),
    _pluginManagerAction(nullptr, "Plugin Manager"),
    _showStartPageAction(nullptr, "Start Page...", true, true)
{
    _newProjectAction.setShortcut(QKeySequence("Ctrl+N"));
    _newProjectAction.setIcon(Application::getIconFont("FontAwesome").getIcon("file"));
    _newProjectAction.setToolTip("Open project from disk");

    _openProjectAction.setShortcut(QKeySequence("Ctrl+O"));
    _openProjectAction.setIcon(Application::getIconFont("FontAwesome").getIcon("folder-open"));
    _openProjectAction.setToolTip("Open project from disk");

    _saveProjectAction.setShortcut(QKeySequence("Ctrl+S"));
    _saveProjectAction.setIcon(Application::getIconFont("FontAwesome").getIcon("save"));
    _saveProjectAction.setToolTip("Save project to disk");

    _saveProjectAsAction.setShortcut(QKeySequence("Ctrl+Shift+S"));
    _saveProjectAsAction.setIcon(Application::getIconFont("FontAwesome").getIcon("save"));
    _saveProjectAsAction.setToolTip("Save project to disk in a chosen location");

    _publishAction.setShortcut(QKeySequence("Ctrl+P"));
    _publishAction.setIcon(Application::getIconFont("FontAwesome").getIcon("share"));
    _publishAction.setToolTip("Publish the HDPS application");

    _pluginManagerAction.setShortcut(QKeySequence("Ctrl+M"));
    _pluginManagerAction.setIcon(Application::getIconFont("FontAwesome").getIcon("plug"));
    _pluginManagerAction.setToolTip("View loaded plugins");

    _showStartPageAction.setShortcut(QKeySequence("Alt+W"));
    _showStartPageAction.setIcon(Application::getIconFont("FontAwesome").getIcon("door-open"));
    _showStartPageAction.setToolTip("Show the HDPS start page");

    auto mainWindow = Application::topLevelWidgets().first();

    mainWindow->addAction(&_newProjectAction);
    mainWindow->addAction(&_openProjectAction);
    mainWindow->addAction(&_saveProjectAction);
    mainWindow->addAction(&_showStartPageAction);

    _importDataMenu.setIcon(Application::getIconFont("FontAwesome").getIcon("file-import"));
    _importDataMenu.setTitle("Import data...");
    _importDataMenu.setToolTip("Import data into HDPS");

    connect(&_importDataMenu, &QMenu::aboutToShow, this, [this]() -> void {
        _importDataMenu.clear();

        for (auto pluginTriggerAction : Application::core()->getPluginManager().getPluginTriggerActions(plugin::Type::LOADER))
            _importDataMenu.addAction(pluginTriggerAction);
    });

    connect(&_newProjectAction, &QAction::triggered, this, [this]() -> void {
        newProject();
    });

    connect(&_openProjectAction, &QAction::triggered, this, [this]() -> void {
        loadProject();
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

    connect(&_pluginManagerAction, &TriggerAction::triggered, this, [this]() -> void {
        PluginManagerDialog::managePlugins();
    });

    const auto updateActionsReadOnly = [this]() -> void {
        _saveProjectAction.setEnabled(!_project.isNull());
        _saveProjectAsAction.setEnabled(!_project.isNull());
        _saveProjectAsAction.setEnabled(!_project.isNull() && !_project->getFilePath().isEmpty());
    };

    connect(this, &ProjectManager::projectCreated, this, updateActionsReadOnly);
    connect(this, &ProjectManager::projectDestroyed, this, updateActionsReadOnly);
    connect(this, &ProjectManager::projectLoaded, this, updateActionsReadOnly);
    connect(this, &ProjectManager::projectSaved, this, updateActionsReadOnly);

    updateActionsReadOnly();

    _recentProjectsAction.initialize("Manager/Project/Recent", "Project", "Ctrl", Application::getIconFont("FontAwesome").getIcon("file"));

    connect(&_recentProjectsAction, &RecentFilesAction::triggered, this, [this](const QString& filePath) -> void {
        loadProject(filePath);
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
        core->getDataManager().reset();
        core->getPluginManager().reset();
        core->getDataHierarchyManager().reset();
        core->getWorkspaceManager().reset();
    }
    endReset();
}

void ProjectManager::newProject()
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
        exceptionMessageBox("Unable to create new project", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to create new project");
    }
}

void ProjectManager::loadProject(QString filePath /*= ""*/)
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
                fileDialog.setWindowTitle("Load project");
                fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
                fileDialog.setFileMode(QFileDialog::ExistingFile);
                fileDialog.setNameFilters({ "HDPS project files (*.hdps)" });
                fileDialog.setDefaultSuffix(".hdps");
                fileDialog.setDirectory(Application::current()->getSetting("Projects/WorkingDirectory", QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)).toString());

                if (fileDialog.exec() == 0)
                    return;

                if (fileDialog.selectedFiles().count() != 1)
                    throw std::runtime_error("Only one file may be selected");

                filePath = fileDialog.selectedFiles().first();

                Application::current()->setSetting("Projects/WorkingDirectory", QFileInfo(filePath).absolutePath());
            }

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

            const auto getSettingsPrefix = [](const QString& projectFilePath) -> QString {
                return "Projects/" + projectFilePath + "/";
            };

            bool            enableCompression = DEFAULT_ENABLE_COMPRESSION;
            std::uint32_t   compressionLevel = DEFAULT_COMPRESSION_LEVEL;

            if (filePath.isEmpty()) {

                QFileDialog fileDialog;

                fileDialog.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("save"));
                fileDialog.setWindowTitle("Save project");
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
    reset();

    _project.reset(new Project());

    emit projectCreated(*(_project.get()));

    _showStartPageAction.setChecked(false);
}

bool ProjectManager::hasProject() const
{
    return getProject() != nullptr;
}

const hdps::Project* ProjectManager::getProject() const
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
