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
    Application(argc, argv)
{
}

void HdpsApplication::loadAnalysis()
{
    try
    {
        // Supplied file path is not valid so create a file dialog for opening a JSON file
        QFileDialog fileDialog;

        // Configure file dialog
        fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
        fileDialog.setFileMode(QFileDialog::ExistingFile);
        fileDialog.setNameFilters({ "HDPS analysis files (*.hdps)" });
        fileDialog.setDefaultSuffix(".hdps");

        // Loading failed when the file dialog is canceled
        if (fileDialog.exec() == 0)
            throw std::runtime_error("File selection was canceled");

        // Only load if we have one file
        if (fileDialog.selectedFiles().count() != 1)
            throw std::runtime_error("Only one file may be selected");

        // Establish the JSON file path that will be loaded
        const auto compressedFilePath = fileDialog.selectedFiles().first();

        // Create temporary dir for decompressed data
        QTemporaryDir temporaryDir;

        // Create archiver for decompression
//        Archiver archiver;
//
//        // Decompress folder to temporary directory
//        archiver.decompressFolder(compressedFilePath, temporaryDir.path());
//
//        // Load JSON when finished
//        connect(&archiver, &Archiver::finished, this, [this, &temporaryDir]() -> void {
//
//#ifdef _VERBOSE
//            qDebug() << QDir(temporaryDir.path()).entryList();
//#endif
//
//            // Set temporary serialization directory so that binaries are loaded from the correct location
//            _serializationTemporaryDirectory = temporaryDir.path();
//
//            // Input JSON file info
//            QFileInfo inputJsonFileInfo(temporaryDir.path(), "analysis.json");
//
//            // Load JSON file from temporary serialization directory
//            _core->fromJsonFile(inputJsonFileInfo.absoluteFilePath());
//        });
//
//        // Show error message when decompressing failed
//        connect(&archiver, &Archiver::error, this, [this, &temporaryDir](const QString& error) -> void {
//            throw std::runtime_error(error.toLatin1());
//        });
    }
    catch (...)
    {
        exceptionMessageBox("Unable to load HDPS analysis");
    }
}

void HdpsApplication::saveAnalysis()
{
    try
    {
        // Create temporary dir for output files
        QTemporaryDir temporaryDir;

        // Create UUID-based output directory name in the temporary directory
        const auto temporaryDirectoryPath = QDir::toNativeSeparators(temporaryDir.path());

#ifdef _VERBOSE
        qDebug().noquote() << QString("Temporary directory: %1").arg(temporaryDirectoryPath);
#endif

        // Supplied file path is not valid so create a file dialog for saving a JSON file
        QFileDialog fileDialog;

        // Configure file dialog
        fileDialog.setAcceptMode(QFileDialog::AcceptSave);
        fileDialog.setNameFilters({ "HDPS analysis files (*.hdps)" });
        fileDialog.setDefaultSuffix(".hdps");
        fileDialog.setOption(QFileDialog::DontUseNativeDialog, true);

        auto fileDialogLayout = dynamic_cast<QGridLayout*>(fileDialog.layout());

        auto rowCount = fileDialogLayout->rowCount();

        QCheckBox   enableCompressionCheckBox("Compression");
        QSpinBox    compressionLevelSpinBox;
        QCheckBox   passwordProtectedCheckBox("Password protected");
        QLineEdit   passwordLineEdit;

        enableCompressionCheckBox.setChecked(false);
        compressionLevelSpinBox.setMinimum(1);
        compressionLevelSpinBox.setMaximum(9);
        compressionLevelSpinBox.setValue(2);
        passwordProtectedCheckBox.setChecked(false);
        passwordLineEdit.setPlaceholderText("Enter encryption password...");

        // Add controls for compression and password protection
        fileDialogLayout->addWidget(&enableCompressionCheckBox, rowCount, 0);
        fileDialogLayout->addWidget(&compressionLevelSpinBox, rowCount, 1);
        fileDialogLayout->addWidget(&passwordProtectedCheckBox, ++rowCount, 0);
        fileDialogLayout->addWidget(&passwordLineEdit, rowCount, 1);

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

        // Saving failed when the file dialog is canceled
        if (fileDialog.exec() == 0)
            throw std::runtime_error("File selection was canceled");

        // Only save if we have one file
        if (fileDialog.selectedFiles().count() != 1)
            throw std::runtime_error("Only one file may be selected");

        // Create an archiver which will be used for directory compression
        Archiver archiver;

        // List of tasks that need to be performed during saving
        QStringList tasks;

        // Create list of tasks
        tasks << "Export data model" << "Temporary task";

        // Create dialog for reporting progress
        TaskProgressDialog taskProgressDialog(nullptr, tasks, "Saving analysis to " + fileDialog.selectedFiles().first(), getIconFont("FontAwesome").getIcon("file-export"));

        // Set current task to JSON + binaries export
        taskProgressDialog.setCurrentTask("Export data model");

        // Output analysis JSON file info
        QFileInfo jsonFileInfo(temporaryDirectoryPath, "analysis.json");

        // Set temporary serialization directory so that binaries are saved in the correct location
        _serializationTemporaryDirectory = temporaryDirectoryPath;

        // Report which item in the hierarchy is being exported
        connect(&_core->getDataHierarchyManager(), &DataHierarchyManager::itemSaving, this, [&taskProgressDialog](DataHierarchyItem& savingItem) {
            taskProgressDialog.setCurrentTask("Exporting " + savingItem.getFullPathName());
        });

        // Write JSON file into temporary serialization directory
        _core->toJsonFile(jsonFileInfo.absoluteFilePath());

        // JSON + binaries export has finished
        taskProgressDialog.setTaskFinished("Export data model");

        // Add tasks for each file in the temporary directory
        taskProgressDialog.addTasks(archiver.getTaskNamesForDirectoryCompression(temporaryDirectoryPath));

        // Remove the temporary task
        taskProgressDialog.setTaskFinished("Temporary task");

        connect(&archiver, &Archiver::taskStarted, &taskProgressDialog, &TaskProgressDialog::setCurrentTask);
        connect(&archiver, &Archiver::taskFinished, &taskProgressDialog, &TaskProgressDialog::setTaskFinished);

        // Compress the entire output directory
        archiver.compressDirectory(temporaryDirectoryPath, QDir::toNativeSeparators(fileDialog.selectedFiles().first()), true, enableCompressionCheckBox.isChecked() ? compressionLevelSpinBox.value() : 0, passwordLineEdit.text());
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to save HDPS analysis", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to save HDPS analysis");
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

void HdpsApplication::TaskProgressDialog::setCurrentTaskName(const QString& taskName)
{
    // Update label text
    setLabelText(taskName);

    // Ensure the progress dialog gets updated
    QCoreApplication::processEvents();
}

}
