#include "HdpsApplication.h"
#include "DataHierarchyManager.h"
#include "Archiver.h"

#include <CoreInterface.h>

#include <util/Exception.h>

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

#define _VERBOSE

using namespace hdps::util;

namespace hdps {


HdpsApplication::HdpsApplication(int& argc, char** argv) :
    Application(argc, argv),
    _enableCompression(getSetting("Projects/EnableCompression", false).toBool()),
    _compressionLevel(getSetting("Projects/CompressionLevel", 2).toInt())
{
}

void HdpsApplication::loadProject(QString projectFilePath /*= ""*/)
{
    try
    {
        // Create temporary dir for intermediate files
        QTemporaryDir temporaryDirectory;

        // Create UUID-based output directory name in the temporary directory
        const auto temporaryDirectoryPath = temporaryDirectory.path();

        // Set the serialization temporary directory so that we can find the binaries
        _serializationTemporaryDirectory = temporaryDirectoryPath;

        // Prompt the user for a file path if the current file path is empty
        if (projectFilePath.isEmpty()) {

            // Create a file dialog for opening an HDPS analysis file
            QFileDialog fileDialog;

            // Configure file dialog
            fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
            fileDialog.setFileMode(QFileDialog::ExistingFile);
            fileDialog.setNameFilters({ "HDPS analysis files (*.hdps)" });
            fileDialog.setDefaultSuffix(".hdps");

            // Loading failed when the file dialog is canceled
            if (fileDialog.exec() == 0)
                return;

            // Only load if we have one file
            if (fileDialog.selectedFiles().count() != 1)
                throw std::runtime_error("Only one file may be selected");

            projectFilePath = fileDialog.selectedFiles().first();
        }

        qDebug().noquote() << "Loading HDPS project from " << projectFilePath;

        // Create archiver for decompression
        Archiver archiver;

        // List of tasks that need to be performed during decompression
        QStringList tasks = archiver.getTaskNamesForDecompression(projectFilePath) << "Import data model";

        // Create dialog for reporting load progress
        TaskProgressDialog taskProgressDialog(nullptr, tasks, "Loading HDPS project from " + projectFilePath, getIconFont("FontAwesome").getIcon("file-import"));

        // Report which item in the hierarchy is being imported
        connect(&_core->getDataHierarchyManager(), &DataHierarchyManager::itemLoading, this, [&taskProgressDialog](DataHierarchyItem& loadingItem) {
            taskProgressDialog.setCurrentTask("Importing dataset: " + loadingItem.getFullPathName());
        });

        // Update task progress dialog when tasks start and finish
        connect(&archiver, &Archiver::taskStarted, &taskProgressDialog, &TaskProgressDialog::setCurrentTask);
        connect(&archiver, &Archiver::taskFinished, &taskProgressDialog, &TaskProgressDialog::setTaskFinished);

        // Decompress folder to temporary directory
        archiver.decompress(projectFilePath, temporaryDirectoryPath);

        // Set current task to data model export
        taskProgressDialog.setCurrentTask("Import data model");

        // Input JSON file info
        QFileInfo inputJsonFileInfo(temporaryDirectoryPath, "analysis.json");

        // Load JSON file from temporary serialization directory
        _core->fromJsonFile(inputJsonFileInfo.absoluteFilePath());

        // Data model import has finished
        taskProgressDialog.setTaskFinished("Import data model");

        // Add saved project to recent projects setting
        addRecentProjectFilePath(projectFilePath);

        // Adjust the current project file path
        setCurrentProjectFilePath(projectFilePath);
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

void HdpsApplication::saveProject(QString projectFilePath /*= ""*/)
{
    try
    {
        // Create temporary dir for intermediate files
        QTemporaryDir temporaryDirectory;

        // Create UUID-based output directory name in the temporary directory
        const auto temporaryDirectoryPath = temporaryDirectory.path();

#ifdef _VERBOSE
        qDebug().noquote() << QString("Temporary directory: %1").arg(temporaryDirectoryPath);
#endif

        // Prompt the user for a file path if the current file path is empty
        if (projectFilePath.isEmpty()) {

            // Create a file dialog for saving an HDPS project file
            QFileDialog fileDialog;

            // Configure file dialog
            fileDialog.setAcceptMode(QFileDialog::AcceptSave);
            fileDialog.setNameFilters({ "HDPS analysis files (*.hdps)" });
            fileDialog.setDefaultSuffix(".hdps");
            fileDialog.setOption(QFileDialog::DontUseNativeDialog, true);

            // Get pointer to the file dialog layout
            auto fileDialogLayout = dynamic_cast<QGridLayout*>(fileDialog.layout());

            auto rowCount = fileDialogLayout->rowCount();

            QCheckBox   enableCompressionCheckBox("Compression:");
            QSpinBox    compressionLevelSpinBox;
            //QCheckBox   passwordProtectedCheckBox("Password protected");
            //QLineEdit   passwordLineEdit;

            enableCompressionCheckBox.setChecked(_enableCompression);
            compressionLevelSpinBox.setPrefix("Level: ");
            compressionLevelSpinBox.setMinimum(1);
            compressionLevelSpinBox.setMaximum(9);
            compressionLevelSpinBox.setValue(_compressionLevel);
            //passwordProtectedCheckBox.setChecked(false);
            //passwordLineEdit.setPlaceholderText("Enter encryption password...");

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
            //const auto updatePassword = [&]() -> void {
            //    passwordLineEdit.setEnabled(passwordProtectedCheckBox.isChecked());
            //};

            connect(&enableCompressionCheckBox, &QCheckBox::toggled, this, updateCompressionLevel);
            //connect(&passwordProtectedCheckBox, &QCheckBox::toggled, this, updatePassword);

            // Initial updates
            updateCompressionLevel();
            //updatePassword();

            // Only save if we have one file
            if (fileDialog.selectedFiles().count() != 1)
                throw std::runtime_error("Only one file may be selected");

            // Update compression settings
            _enableCompression  = enableCompressionCheckBox.isChecked();
            _compressionLevel   = compressionLevelSpinBox.value();

            // Update the project file path
            projectFilePath = fileDialog.selectedFiles().first();
        }

        qDebug().noquote() << "Saving HDPS project to " << projectFilePath;

        // Create an archiver which will be used for directory compression
        Archiver archiver;

        // List of tasks that need to be performed during saving
        QStringList tasks;

        // Create list of tasks
        tasks << "Export data model" << "Temporary task";

        // Create dialog for reporting save progress
        TaskProgressDialog taskProgressDialog(nullptr, tasks, "Saving analysis to " + projectFilePath, getIconFont("FontAwesome").getIcon("file-export"));

        // Set current task to data model export
        taskProgressDialog.setCurrentTask("Export data model");

        // Output analysis JSON file info
        QFileInfo jsonFileInfo(temporaryDirectoryPath, "analysis.json");

        // Set temporary serialization directory so that binaries are saved in the correct location
        _serializationTemporaryDirectory = temporaryDirectoryPath;

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
        archiver.compressDirectory(temporaryDirectoryPath, projectFilePath, true, _enableCompression ? _compressionLevel : 0, "");

        // Add saved project to recent projects setting
        addRecentProjectFilePath(projectFilePath);

        // Save settings
        setSetting("Projects/EnableCompression", _enableCompression);
        setSetting("Projects/CompressionLevel", _compressionLevel);

        // Adjust the current project file path
        setCurrentProjectFilePath(projectFilePath);
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
