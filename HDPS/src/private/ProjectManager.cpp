#include "ProjectManager.h"
#include "Archiver.h"

#include <Application.h>

#include <util/Exception.h>

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
    _newProjectAction(this, "New Project"),
    _openProjectAction(this, "Open Project"),
    _saveProjectAction(this, "Save Project"),
    _saveProjectAsAction(this, "Save Project As..."),
    _recentProjectsMenu()
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

    _saveProjectAsAction.setIcon(Application::getIconFont("FontAwesome").getIcon("save"));
    _saveProjectAsAction.setToolTip("Save project to disk in a chosen location");

    _recentProjectsMenu.setTitle("Recent projects...");
    _recentProjectsMenu.setToolTip("Recently opened HDPS projects");
    _recentProjectsMenu.setIcon(Application::getIconFont("FontAwesome").getIcon("clock"));

    auto mainWindow = Application::topLevelWidgets().first();

    mainWindow->addAction(&_newProjectAction);
    mainWindow->addAction(&_openProjectAction);
    mainWindow->addAction(&_saveProjectAction);

    updateRecentProjectsMenu();

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

    const auto updateActionsReadOnly = [this]() -> void {
        _saveProjectAction.setEnabled(!_project.isNull());
        _saveProjectAsAction.setEnabled(!_project.isNull());
        _saveProjectAsAction.setEnabled(!_project.isNull());
    };

    connect(this, &ProjectManager::projectCreated, this, updateActionsReadOnly);
    connect(this, &ProjectManager::projectDestroyed, this, updateActionsReadOnly);

    updateActionsReadOnly();
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

            _serializationTemporaryDirectory = temporaryDirectoryPath;

            _serializationAborted = false;

            if (filePath.isEmpty()) {
                QFileDialog fileDialog;

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
                _serializationAborted = true;

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

            Application::core()->fromJsonFile(inputJsonFileInfo.absoluteFilePath());

            taskProgressDialog.setTaskFinished("Import data model");

            addRecentProjectFilePath(filePath);

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
                _serializationAborted = true;

                throw std::runtime_error("Canceled before project was saved");
                });

            QFileInfo jsonFileInfo(temporaryDirectoryPath, "project.json");

            _serializationTemporaryDirectory = temporaryDirectoryPath;

            _serializationAborted = false;

            connect(&Application::core()->getDataHierarchyManager(), &AbstractDataHierarchyManager::itemSaving, this, [&taskProgressDialog](DataHierarchyItem& savingItem) {
                taskProgressDialog.setCurrentTask("Exporting dataset: " + savingItem.getFullPathName());
                });

            Application::core()->toJsonFile(jsonFileInfo.absoluteFilePath());

            taskProgressDialog.setTaskFinished("Export data model");
            taskProgressDialog.addTasks(archiver.getTaskNamesForDirectoryCompression(temporaryDirectoryPath));
            taskProgressDialog.setTaskFinished("Temporary task");

            connect(&archiver, &Archiver::taskStarted, &taskProgressDialog, &TaskProgressDialog::setCurrentTask);
            connect(&archiver, &Archiver::taskFinished, &taskProgressDialog, &TaskProgressDialog::setTaskFinished);

            archiver.compressDirectory(temporaryDirectoryPath, filePath, true, enableCompression ? compressionLevel : 0, "");

            addRecentProjectFilePath(filePath);

            Application::current()->setSetting(getSettingsPrefix(filePath) + "EnableCompression", enableCompression);
            Application::current()->setSetting(getSettingsPrefix(filePath) + "CompressionLevel", compressionLevel);

            _project->setFilePath(filePath);

            updateRecentProjectsMenu();

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

QMenu* ProjectManager::getRecentProjectsMenu()
{
    return &_recentProjectsMenu;
}

void ProjectManager::updateRecentProjectsMenu()
{
    _recentProjectsMenu.clear();

    const auto recentProjects = Application::current()->getSetting("Projects/Recent", QVariantList()).toList();

    _recentProjectsMenu.setEnabled(!recentProjects.isEmpty());

    for (const auto& recentProject : recentProjects) {
        const auto recentProjectFilePath = recentProject.toMap()["FilePath"].toString();

        if (!QFileInfo(recentProjectFilePath).exists())
            continue;

        auto recentProjectAction = new QAction(recentProjectFilePath);

        recentProjectAction->setIcon(Application::getIconFont("FontAwesome").getIcon("file"));
        recentProjectAction->setToolTip("Load " + recentProjectFilePath + "(last opened on " + recentProject.toMap()["DateTime"].toDate().toString() + ")");

        connect(recentProjectAction, &QAction::triggered, this, [recentProjectFilePath]() -> void {
            Application::core()->getProjectManager().loadProject(recentProjectFilePath);
        });

        _recentProjectsMenu.addAction(recentProjectAction);
    }
}

void ProjectManager::createProject()
{
    reset();

    _project.reset(new Project());

    emit projectCreated(*(_project.get()));
}

void ProjectManager::addRecentProjectFilePath(const QString& filePath)
{
    auto recentProjects = Application::current()->getSetting("Projects/Recent", QVariantList()).toList();

    QVariantMap recentProject{
        { "FilePath", filePath },
        { "DateTime", QDateTime::currentDateTime() }
    };

    for (auto recentProject : recentProjects)
        if (recentProject.toMap()["FilePath"].toString() == filePath)
            recentProjects.removeOne(recentProject);

    recentProjects.insert(0, recentProject);

    Application::current()->setSetting("Projects/Recent", recentProjects);
}

hdps::Project* ProjectManager::getCurrentProject()
{
    return _project.get();
}
