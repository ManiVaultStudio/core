#include "HdpsApplication.h"
#include "DataHierarchyManager.h"
#include "Archiver.h"

#include <CoreInterface.h>

#include <util/Exception.h>
#include <actions/DataRemoveAction.h>

#include <QDebug>
#include <QTemporaryDir>
#include <QFileDialog>
#include <QDir>
#include <QGridLayout>
#include <QCheckBox>
#include <QLabel>
#include <QSpinBox>
#include <QLineEdit>
#include <QTemporaryDir>
#include <QStandardPaths>

#define _VERBOSE

using namespace hdps::util;

namespace hdps {

#if (__cplusplus < 201703L)   // definition needed for pre C++17 gcc and clang
    constexpr bool HdpsApplication::DEFAULT_ENABLE_COMPRESSION;
    constexpr std::uint32_t HdpsApplication::DEFAULT_COMPRESSION_LEVEL;
#endif

HdpsApplication::HdpsApplication(int& argc, char** argv) :
    Application(argc, argv)
{
}

void HdpsApplication::loadProject(QString projectFilePath /*= ""*/)
{
    try
    {
        // Except if the supplied project file path is a directory
        if (QFileInfo(projectFilePath).isDir())
            throw std::runtime_error("Project file path may not be a directory");

        // Get all loaded datasets (irrespective of the data type)
        const auto loadedDatasets = _core->requestAllDataSets();

        // The project needs to be cleared if there are one or more datasets loaded
        if (!loadedDatasets.empty()) {

            // Check in the settings if the user has to be prompted with a question whether to automatically remove all datasets
            if (Application::current()->getSetting("ConfirmDataRemoval", true).toBool()) {

                // Ask for confirmation dialog
                DataRemoveAction::ConfirmDataRemoveDialog confirmDataRemoveDialog(nullptr, "Data model will reset", loadedDatasets);

                // Show the confirm data removal dialog
                confirmDataRemoveDialog.exec();

                // Remove dataset and children from the core if accepted
                if (confirmDataRemoveDialog.result() == 1)
                    Application::core()->removeAllDatasets();
                else
                    return;
            }
        }

        // Create temporary dir for intermediate files
        QTemporaryDir temporaryDirectory;

        // Create UUID-based output directory name in the temporary directory
        const auto temporaryDirectoryPath = temporaryDirectory.path();

        // Set the serialization temporary directory so that we can find the binaries
        _serializationTemporaryDirectory = temporaryDirectoryPath;

        // Reset serialization aborted state
        _serializationAborted = false;

        // Prompt the user for a file path if the current file path is empty
        if (projectFilePath.isEmpty()) {

            // Create a file dialog for opening an HDPS project file
            QFileDialog fileDialog;

            // Configure file dialog
            fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
            fileDialog.setFileMode(QFileDialog::ExistingFile);
            fileDialog.setNameFilters({ "HDPS project files (*.hdps)" });
            fileDialog.setDefaultSuffix(".hdps");
            fileDialog.setDirectory(getSetting("Projects/WorkingDirectory", QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)).toString());

            // Loading failed when the file dialog is canceled
            if (fileDialog.exec() == 0)
                return;

            // Only load if we have one file
            if (fileDialog.selectedFiles().count() != 1)
                throw std::runtime_error("Only one file may be selected");

            // Get the project file path
            projectFilePath = fileDialog.selectedFiles().first();

            // Update the projects working directory
            setSetting("Projects/WorkingDirectory", QFileInfo(projectFilePath).absolutePath());
        }

        qDebug().noquote() << "Loading HDPS project from" << projectFilePath;

        // Create archiver for decompression
        Archiver archiver;

        // List of tasks that need to be performed during decompression
        QStringList tasks = archiver.getTaskNamesForDecompression(projectFilePath) << "Import data model";

        // Create dialog for reporting load progress
        TaskProgressDialog taskProgressDialog(nullptr, tasks, "Loading HDPS project from " + projectFilePath, getIconFont("FontAwesome").getIcon("folder-open"));

        // Throw an exception when project open is canceled
        connect(&taskProgressDialog, &TaskProgressDialog::canceled, this, [this]() -> void {
            _serializationAborted = true;

            throw std::runtime_error("Canceled before project was loaded");
        });

        // Report which item in the hierarchy is being imported
        connect(&_core->getDataHierarchyManager(), &DataHierarchyManager::itemLoading, this, [&taskProgressDialog](DataHierarchyItem& loadingItem) {
            //taskProgressDialog.setCurrentTask("Importing dataset: " + loadingItem.getFullPathName());
        });

        // Update task progress dialog when tasks start and finish
        connect(&archiver, &Archiver::taskStarted, &taskProgressDialog, &TaskProgressDialog::setCurrentTask);
        connect(&archiver, &Archiver::taskFinished, &taskProgressDialog, &TaskProgressDialog::setTaskFinished);

        // Decompress folder to temporary directory
        archiver.decompress(projectFilePath, temporaryDirectoryPath);

        // Set current task to data model export
        taskProgressDialog.setCurrentTask("Import data model");

        // Input JSON file info
        QFileInfo inputJsonFileInfo(temporaryDirectoryPath, "project.json");

        // Load JSON file from temporary serialization directory
        _core->fromJsonFile(inputJsonFileInfo.absoluteFilePath());

        // Data model import has finished
        taskProgressDialog.setTaskFinished("Import data model");

        // Add saved project to recent projects setting
        addRecentProjectFilePath(projectFilePath);

        // Adjust the current project file path
        setCurrentProjectFilePath(projectFilePath);

        qDebug().noquote() << projectFilePath << "loaded successfully";
    }
    catch (std::exception& e)
    {
        _core->removeAllDatasets();

        exceptionMessageBox("Unable to load HDPS project", e);
    }
    catch (...)
    {
        _core->removeAllDatasets();

        exceptionMessageBox("Unable to load HDPS project");
    }
}

void HdpsApplication::saveProject(QString projectFilePath /*= ""*/)
{
    try
    {
        // Except if the supplied project file path is a directory
        if (QFileInfo(projectFilePath).isDir())
            throw std::runtime_error("Project file path may not be a directory");

        // Create temporary dir for intermediate files
        QTemporaryDir temporaryDirectory;

        // Create UUID-based output directory name in the temporary directory
        const auto temporaryDirectoryPath = temporaryDirectory.path();

        // Settings prefix
        const auto getSettingsPrefix = [](const QString& projectFilePath) -> QString {
            return "Projects/" + projectFilePath + "/";
        };

        // Default compression settings
        bool            enableCompression   = DEFAULT_ENABLE_COMPRESSION;
        std::uint32_t   compressionLevel    = DEFAULT_COMPRESSION_LEVEL;

        // Prompt the user for a file path if the current file path is empty
        if (projectFilePath.isEmpty()) {

            // Create a file dialog for saving an HDPS project file
            QFileDialog fileDialog;

            // Configure file dialog
            fileDialog.setAcceptMode(QFileDialog::AcceptSave);
            fileDialog.setNameFilters({ "HDPS project files (*.hdps)" });
            fileDialog.setDefaultSuffix(".hdps");
            fileDialog.setOption(QFileDialog::DontUseNativeDialog, true);
            fileDialog.setDirectory(getSetting("Projects/WorkingDirectory", QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)).toString());

            // Get pointer to the file dialog layout
            auto fileDialogLayout = dynamic_cast<QGridLayout*>(fileDialog.layout());

            // Current row count in the grid layout
            auto rowCount = fileDialogLayout->rowCount();

            // Create additional user interface elements
            QCheckBox   enableCompressionCheckBox("Compression:");
            QSpinBox    compressionLevelSpinBox;
            QCheckBox   passwordProtectedCheckBox("Password protected");
            QLineEdit   passwordLineEdit;

            // Configure user interface elements
            enableCompressionCheckBox.setChecked(enableCompression);
            compressionLevelSpinBox.setPrefix("Level: ");
            compressionLevelSpinBox.setMinimum(1);
            compressionLevelSpinBox.setMaximum(9);
            compressionLevelSpinBox.setValue(compressionLevel);
            passwordProtectedCheckBox.setChecked(false);
            passwordLineEdit.setPlaceholderText("Enter encryption password...");

            // Add controls for compression and password protection
            fileDialogLayout->addWidget(&enableCompressionCheckBox, rowCount, 0);
            fileDialogLayout->addWidget(&compressionLevelSpinBox, rowCount, 1);
            //fileDialogLayout->addWidget(&passwordProtectedCheckBox, ++rowCount, 0);
            //fileDialogLayout->addWidget(&passwordLineEdit, rowCount, 1);

            // Update read only state of the compression level control
            const auto updateCompressionLevel = [&]() -> void {
                compressionLevelSpinBox.setEnabled(enableCompressionCheckBox.isChecked());
            };

            // Update read only state of the password control
            const auto updatePassword = [&]() -> void {
                passwordLineEdit.setEnabled(passwordProtectedCheckBox.isChecked());
            };

            connect(&enableCompressionCheckBox, &QCheckBox::toggled, this, updateCompressionLevel);
            connect(&passwordProtectedCheckBox, &QCheckBox::toggled, this, updatePassword);

            // Initial updates
            updateCompressionLevel();
            updatePassword();

            // Update compression parameters when the current file path changes
            connect(&fileDialog, &QFileDialog::currentChanged, this, [this, getSettingsPrefix, enableCompression, compressionLevel, &enableCompressionCheckBox, &compressionLevelSpinBox](const QString& path) -> void {
                enableCompressionCheckBox.setChecked(getSetting(getSettingsPrefix(path) + "EnableCompression", enableCompression).toBool());
                compressionLevelSpinBox.setValue(getSetting(getSettingsPrefix(path) + "CompressionLevel", compressionLevel).toInt());
            });

            // Show the dialog
            fileDialog.exec();

            // Only save if we have one file
            if (fileDialog.selectedFiles().count() != 1)
                throw std::runtime_error("Only one file may be selected");

            // Update the project file path
            projectFilePath = fileDialog.selectedFiles().first();

            // File-specific compression settings
            enableCompression   = enableCompressionCheckBox.isChecked();
            compressionLevel    = compressionLevelSpinBox.value();

            // Update the projects working directory
            setSetting("Projects/WorkingDirectory", QFileInfo(projectFilePath).absolutePath());
        }
        else
        {
            enableCompression   = getSetting(getSettingsPrefix(projectFilePath) + "EnableCompression", enableCompression).toBool();
            compressionLevel    = getSetting(getSettingsPrefix(projectFilePath) + "CompressionLevel", compressionLevel).toInt();
        }

        // Last sanity check
        if (projectFilePath.isEmpty() || QFileInfo(projectFilePath).isDir())
            return;

        if (enableCompression)
            qDebug().noquote() << "Saving HDPS project to" << projectFilePath << "with compression level" << compressionLevel;
        else
            qDebug().noquote() << "Saving HDPS project to" << projectFilePath << "without compression";

        // Create an archiver which will be used for directory compression
        Archiver archiver;

        // List of tasks that need to be performed during saving
        QStringList tasks;

        // Create list of tasks
        tasks << "Export data model" << "Temporary task";

        // Create dialog for reporting save progress
        TaskProgressDialog taskProgressDialog(nullptr, tasks, "Saving HDPS project to " + projectFilePath, getIconFont("FontAwesome").getIcon("save"));

        // Set current task to data model export
        taskProgressDialog.setCurrentTask("Export data model");

        // Throw an exception when project save is canceled
        connect(&taskProgressDialog, &TaskProgressDialog::canceled, this, [this]() -> void {
            _serializationAborted = true;

            throw std::runtime_error("Canceled before project was saved");
        });

        // Output project JSON file info
        QFileInfo jsonFileInfo(temporaryDirectoryPath, "project.json");

        // Set temporary serialization directory so that binaries are saved in the correct location
        _serializationTemporaryDirectory = temporaryDirectoryPath;

        // Reset serialization aborted state
        _serializationAborted = false;

        // Report which item in the hierarchy is being exported
        connect(&_core->getDataHierarchyManager(), &DataHierarchyManager::itemSaving, this, [&taskProgressDialog](DataHierarchyItem& savingItem) {
            taskProgressDialog.setCurrentTask("Exporting dataset: " + savingItem.getFullPathName());
        });

        // Write JSON file into temporary serialization directory
        _core->toJsonFile(jsonFileInfo.absoluteFilePath());

        // Data model export has finished
        taskProgressDialog.setTaskFinished("Export data model");

        // Add tasks for each file in the temporary directory
        taskProgressDialog.addTasks(archiver.getTaskNamesForDirectoryCompression(temporaryDirectoryPath));

        // Remove the temporary task
        taskProgressDialog.setTaskFinished("Temporary task");

        connect(&archiver, &Archiver::taskStarted, &taskProgressDialog, &TaskProgressDialog::setCurrentTask);
        connect(&archiver, &Archiver::taskFinished, &taskProgressDialog, &TaskProgressDialog::setTaskFinished);

        // Compress the entire output directory
        archiver.compressDirectory(temporaryDirectoryPath, projectFilePath, true, enableCompression ? compressionLevel : 0, "");

        // Add saved project to recent projects setting
        addRecentProjectFilePath(projectFilePath);

        // Save settings
        setSetting(getSettingsPrefix(projectFilePath) + "EnableCompression", enableCompression);
        setSetting(getSettingsPrefix(projectFilePath) + "CompressionLevel", compressionLevel);

        // Adjust the current project file path
        setCurrentProjectFilePath(projectFilePath);

        qDebug().noquote() << projectFilePath << "saved successfully";
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to save HDPS project", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to save HDPS project");
    }
}

HdpsApplication::TaskProgressDialog::TaskProgressDialog(QWidget* parent, const QStringList& tasks, const QString& title, const QIcon& icon) :
    QProgressDialog(parent),
    _tasks(tasks)
{
    setWindowIcon(icon);
    setWindowTitle(title);
    setWindowModality(Qt::WindowModal);
    setMinimumDuration(0);
    setFixedWidth(600);
    setMinimum(0);
    setMaximum(_tasks.count());
    setValue(0);
    setAutoClose(false);
    setAutoReset(false);
}

void HdpsApplication::TaskProgressDialog::addTasks(const QStringList& tasks)
{
    _tasks << tasks;

    setMaximum(_tasks.count());
}

void HdpsApplication::TaskProgressDialog::setCurrentTask(const QString& taskName)
{
    // Update the label text
    setLabelText(taskName);

    // Ensure the progress dialog gets updated
    QCoreApplication::processEvents();
}

void HdpsApplication::TaskProgressDialog::setTaskFinished(const QString& taskName)
{
    // Remove the task from the list
    _tasks.removeOne(taskName);

    // Update the progress value
    setValue(maximum() - _tasks.count());

    // Ensure the progress dialog gets updated
    QCoreApplication::processEvents();
}

void HdpsApplication::TaskProgressDialog::setCurrentTaskText(const QString& taskText)
{
    // Update label text
    setLabelText(taskText);

    // Ensure the progress dialog gets updated
    QCoreApplication::processEvents();
}

}
