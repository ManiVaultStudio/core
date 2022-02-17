#pragma once
 
#include <QFile>
#include <QObject>
#include <QDir>
#include <QDataStream>

/**
 * Folder compression class
 *
 * Class for compressing/decompressing folders
 *
 * @author Thomas Kroes
 */
class FolderCompressor : public QObject
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    explicit FolderCompressor(QObject* parent = 0);
 
    /**
     * A recursive function that scans all files inside the source folder 
     * and serializes all files in a row of file names and compressed binary data in a single file
     * @param sourceFolder Source folder
     * @param destinationFile Destination file
     * @return whether successfully saved
     */
    bool compressFolder(const QString& sourceFolder, const QString& destinationFile);
 
    /**
     * A function that de-serializes data from the compressed file and creates any needed subfolders before saving the file
     * @param sourceFolder Source folder
     * @param destinationFile Destination file
     * @return Whether successfully de-serialized
     */
    bool decompressFolder(const QString& sourceFile, const QString& destinationFolder);

protected:

    /**
     * Compress source folder
     * @param sourceFolder Source folder path
     * @param prefix Prefix
     * @return Whether successfully compressed
     */
    bool compress(const QString& sourceFolder, const QString& prefix);

private:
    QFile           _file;          /** In-output file */
    QDataStream     _dataStream;    /** Data stream */

};
 