#include "HdpsApplication.h"
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

#define _VERBOSE

using namespace hdps::util;

namespace hdps {


HdpsApplication::HdpsApplication(int& argc, char** argv) :
    Application(argc, argv),
    _temporaryDir()
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
        _temporaryDir = QSharedPointer<QTemporaryDir>::create();

        // Create UUID-based output directory name in the temporary directory
        const auto temporaryDirectoryPath = QDir::toNativeSeparators(_temporaryDir->path());

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
        compressionLevelSpinBox.setMinimum(0);
        compressionLevelSpinBox.setMaximum(9);
        compressionLevelSpinBox.setValue(7);
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

        // Output analysis JSON file info
        QFileInfo jsonFileInfo(temporaryDirectoryPath, "analysis.json");

        // Set temporary serialization directory so that binaries are saved in the correct location
        _serializationTemporaryDirectory = temporaryDirectoryPath;

        // Write JSON file into temporary serialization directory
        _core->toJsonFile(jsonFileInfo.absoluteFilePath());

#ifdef _VERBOSE
        qDebug() << QDir(temporaryDirectoryPath).entryList((QStringList() << "*.json" << "*.bin", QDir::Files));
#endif

        // Compress the entire output directory
        Archiver::compressDirectory(temporaryDirectoryPath, QDir::toNativeSeparators(fileDialog.selectedFiles().first()), true, enableCompressionCheckBox.isChecked() ? compressionLevelSpinBox.value() : 0, passwordLineEdit.text());
    }
    catch (...)
    {
        exceptionMessageBox("Unable to save HDPS analysis");
    }
}

}
