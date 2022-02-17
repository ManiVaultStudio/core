#include "FolderCompressor.h"

FolderCompressor::FolderCompressor(QObject* parent) :
    QObject(parent)
{
}

bool FolderCompressor::compressFolder(const QString& sourceFolder, const QString& destinationFile)
{
    QDir src(sourceFolder);
    if (!src.exists())//folder not found
    {
        return false;
    }

    _file.setFileName(destinationFile);
    if (!_file.open(QIODevice::WriteOnly))//could not open file
    {
        return false;
    }

    _dataStream.setDevice(&_file);

    bool success = compress(sourceFolder, "");
    _file.close();

    return success;
}

bool FolderCompressor::compress(const QString& sourceFolder, const QString& prefix)
{
    QDir dir(sourceFolder);

    if (!dir.exists())
        return false;

    // List all folders inside the current folder
    dir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);

    QFileInfoList foldersList = dir.entryInfoList();

    // For each folder in list: call the same function with folders' paths
    for (int i = 0; i < foldersList.length(); i++) {
        QString folderName  = foldersList.at(i).fileName();
        QString folderPath  = dir.absolutePath() + "/" + folderName;
        QString newPrefex   = prefix + "/" + folderName;

        compress(folderPath, newPrefex);
    }

    // List all files inside the current folder
    dir.setFilter(QDir::NoDotAndDotDot | QDir::Files);

    QFileInfoList filesList = dir.entryInfoList();

    // For each file in list: add file path and compressed binary data
    for (int i = 0; i < filesList.length(); i++) {
        QFile file(dir.absolutePath() + "/" + filesList.at(i).fileName());

        if (!file.open(QIODevice::ReadOnly))
            return false;

        _dataStream << QString(prefix + "/" + filesList.at(i).fileName());
        _dataStream << qCompress(file.readAll());

        file.close();
    }

    return true;
}

bool FolderCompressor::decompressFolder(const QString& sourceFile, const QString& destinationFolder)
{
    // Validation
    QFile src(sourceFile);

    if (!src.exists())
        return false;

    QDir dir;

    if (!dir.mkpath(destinationFolder))
        return false;

    _file.setFileName(sourceFile);

    if (!_file.open(QIODevice::ReadOnly))
        return false;

    _dataStream.setDevice(&_file);

    while (!_dataStream.atEnd())
    {
        QString fileName;
        QByteArray data;

        // Extract file name and data in order
        _dataStream >> fileName >> data;

        // Create any needed folder
        QString subFolder;

        for (int i = fileName.length() - 1; i > 0; i--) {
            if ((QString(fileName.at(i)) == QString("\\")) || (QString(fileName.at(i)) == QString("/"))) {
                subFolder = fileName.left(i);
                dir.mkpath(destinationFolder + "/" + subFolder);
                break;
            }
        }

        QFile outFile(destinationFolder + "/" + fileName);
        
        if (!outFile.open(QIODevice::WriteOnly)) {
            _file.close();

            return false;
        }

        outFile.write(qUncompress(data));
        outFile.close();
    }

    _file.close();

    return true;
}