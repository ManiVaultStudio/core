#include "Application.h"

#include "CoreInterface.h"
#include "util/FontAwesome.h"
#include "util/Exception.h"
#include "util/FolderCompressor.h"
#include "actions/WidgetAction.h"

#include <stdexcept>

#include <QDebug>
#include <QMessageBox>
#include <QTemporaryDir>
#include <QFileDialog>
#include <QDir>

#define _VERBOSE

using namespace hdps::gui;
using namespace hdps::util;

namespace hdps {

hdps::Application::Application(int& argc, char** argv) :
    QApplication(argc, argv),
    _iconFonts(),
    _settings()
{
    _iconFonts.add(QSharedPointer<IconFont>(new FontAwesome(5, 14)));
}

hdps::Application* hdps::Application::current()
{
    try
    {
        auto hdpsApplication = dynamic_cast<Application*>(instance());

        if (hdpsApplication == nullptr)
            throw std::runtime_error("Current application instance is not an HDPS application");

        return hdpsApplication;
    }
    catch (std::exception& e)
    {
        QMessageBox::critical(nullptr, "An application error occurred", e.what());
        return nullptr;
    }
}

const IconFont& hdps::Application::getIconFont(const QString& name, const std::int32_t& majorVersion /*= -1*/, const std::int32_t& minorVersion /*= -1*/)
{
    return current()->_iconFonts.getIconFont(name, majorVersion, minorVersion);
}

hdps::CoreInterface* Application::getCore()
{
    Q_ASSERT(_core != nullptr);

    return _core;
}

void Application::setCore(CoreInterface* core)
{
    Q_ASSERT(_core != nullptr);

    _core = core;
}

hdps::CoreInterface* Application::core()
{
    return current()->getCore();
}

QVariant Application::getSetting(const QString& path, const QVariant& defaultValue /*= QVariant()*/) const
{
    return _settings.value(path, defaultValue);
}

void Application::setSetting(const QString& path, const QVariant& value)
{
    _settings.setValue(path, value);
}

void Application::loadAnalysis()
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
        const auto filePath = fileDialog.selectedFiles().first();

        // Create temporary dir for decompressed data
        QTemporaryDir temporaryDir;

        // Create folder compressor
        FolderCompressor folderCompressor(this);

        // Decompress folder to temporary directory
        if (!folderCompressor.decompressFolder(filePath, temporaryDir.path()))
            throw std::runtime_error("Unable to decompress folder");

#ifdef _VERBOSE
        qDebug() << QDir(temporaryDir.path()).entryList();
#endif

        // Set temporary serialization directory so that binaries are loaded from the correct location
        _serializationTemporaryDirectory = temporaryDir.path();

        // Input JSON file info
        QFileInfo inputJsonFileInfo(temporaryDir.path(), "analysis.json");

        // Load JSON file from temporary serialization directory
        _core->fromJsonFile(inputJsonFileInfo.absoluteFilePath());
    }
    catch (...)
    {
        exceptionMessageBox("Unable to load HDPS analysis");
    }
}

void Application::saveAnalysis()
{
    try
    {
        // Create temporary dir for output files
        QTemporaryDir temporaryDir;

        // Create UUID-based output directory name in the temporary directory
        const auto outputDirName = temporaryDir.path();

#ifdef _VERBOSE
        qDebug().noquote() << QString("Temporary directory: %1").arg(outputDirName);
#endif

        // Supplied file path is not valid so create a file dialog for saving a JSON file
        QFileDialog fileDialog;

        // Configure file dialog
        fileDialog.setAcceptMode(QFileDialog::AcceptSave);
        fileDialog.setNameFilters({ "HDPS analysis files (*.hdps)" });
        fileDialog.setDefaultSuffix(".hdps");

        // Saving failed when the file dialog is canceled
        if (fileDialog.exec() == 0)
            throw std::runtime_error("File selection was canceled");

        // Only save if we have one file
        if (fileDialog.selectedFiles().count() != 1)
            throw std::runtime_error("Only one file may be selected");

        // Output analysis JSON file info
        QFileInfo jsonFileInfo(outputDirName, "analysis.json");

        // Set temporary serialization directory so that binaries are saved in the correct location
        _serializationTemporaryDirectory = outputDirName;

        // Write JSON file into temporary serialization directory
        _core->toJsonFile(jsonFileInfo.absoluteFilePath());

#ifdef _VERBOSE
        qDebug() << QDir(temporaryDir.path()).entryList();
#endif

        // Establish the JSON file path that will be saved
        const auto outputFileInfo = QFileInfo(fileDialog.selectedFiles().first());

        // Create folder compressor
        FolderCompressor folderCompressor(this);

        // Compress the entire directory and save at the picked location
        if (!folderCompressor.compressFolder(outputDirName, outputFileInfo.fileName()))
            throw std::runtime_error("Unable to compress folder");
    }
    catch (...)
    {
        exceptionMessageBox("Unable to save HDPS analysis");
    }
}

QString Application::getSerializationTemporaryDirectory()
{
    return current()->_serializationTemporaryDirectory;
}

}
