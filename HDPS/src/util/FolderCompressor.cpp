#include "FolderCompressor.h"

#include <QArchive>
#include <QDir>

using QArchive::DiskCompressor;

namespace hdps {

namespace util {

bool compressFolder(const QString& destinationFile, const QString& sourceDirectory, const QString& password /*= ""*/)
{
    //JlCompress::compressDir(destinationFile, sourceDirectory);

    /* Construct DiskCompressor Object. */
    DiskCompressor compressor(destinationFile);

    // Get the JSON file and binaries
    QStringList jsonFileAndBinaries = QDir(sourceDirectory).entryList(QStringList() << "*.json" << "*.bin", QDir::Files);

    // Add file paths to the compressor
    compressor.addFiles(jsonFileAndBinaries);

    QObject::connect(&compressor, &DiskCompressor::finished, [&]() {
        qInfo() << "Compressed File(s) Successfully!";
        return;
    });
    
    /*
    QObject::connect(&Compressor, &DiskCompressor::error,
        [&](short code, QString file) {
        qInfo() << "An error has occured ::"
            << QArchive::errorCodeToString(code)
            << "::"
            << file;
        app.quit();
        return;
    });
    */

    compressor.start();

    return true;
}

bool decompressFolder(const QString& compressedFile, const QString& destinationDirectory, const QString& password /*= ""*/)
{
    //JlCompress::extractDir(compressedFile, destinationDirectory);

    return true;
}

}
}
