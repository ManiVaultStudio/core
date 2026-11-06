#include "Archiver.h"

#include <quazip/JlCompress.h>
#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTemporaryDir>

#include <cstdio>

namespace {

bool require(bool condition, const QString& message)
{
    if (!condition)
        std::fprintf(stderr, "%s\n", message.toLocal8Bit().constData());

    return condition;
}

bool createArchive(const QString& archivePath, const QString& entryName, const QByteArray& contents)
{
    QuaZip zip(archivePath);

    if (!require(zip.open(QuaZip::mdCreate), QStringLiteral("Unable to create test archive")))
        return false;

    QuaZipFile file(&zip);

    if (!require(file.open(QIODevice::WriteOnly, QuaZipNewInfo(entryName)), QStringLiteral("Unable to create test archive entry")))
        return false;

    if (!require(file.write(contents) == contents.size(), QStringLiteral("Unable to write test archive entry")))
        return false;

    file.close();
    zip.close();

    return require(zip.getZipError() == UNZ_OK, QStringLiteral("Test archive was not closed successfully"));
}

bool extractsValidEntries()
{
    QTemporaryDir temporaryDirectory;
    const auto archivePath = QDir(temporaryDirectory.path()).filePath(QStringLiteral("valid.zip"));
    const auto extractedPath = QDir(temporaryDirectory.path()).filePath(QStringLiteral("extracted.txt"));

    if (!require(temporaryDirectory.isValid(), QStringLiteral("Unable to create temporary test directory"))
        || !createArchive(archivePath, QStringLiteral("nested/file.txt"), QByteArrayLiteral("safe")))
        return false;

    mv::util::Archiver archiver;
    archiver.extractSingleFile(archivePath, QStringLiteral("nested/file.txt"), extractedPath);

    if (!require(QFileInfo::exists(extractedPath), QStringLiteral("Archive entry could not be extracted")))
        return false;

    QFile extracted(extractedPath);

    return require(extracted.open(QIODevice::ReadOnly), QStringLiteral("Valid archive entry was not extracted"))
        && require(extracted.readAll() == QByteArrayLiteral("safe"), QStringLiteral("Extracted entry contents are incorrect"));
}

bool rejectsTraversalEntries()
{
    QTemporaryDir temporaryDirectory;
    const auto archivePath = QDir(temporaryDirectory.path()).filePath(QStringLiteral("traversal.zip"));
    const auto destination  = QDir(temporaryDirectory.path()).filePath(QStringLiteral("destination"));
    const auto outsidePath  = QDir(temporaryDirectory.path()).filePath(QStringLiteral("outside.txt"));

    if (!require(temporaryDirectory.isValid(), QStringLiteral("Unable to create temporary test directory"))
        || !createArchive(archivePath, QStringLiteral("../outside.txt"), QByteArrayLiteral("must not escape")))
        return false;

    mv::util::Archiver archiver;
    archiver.decompress(archivePath, destination);

    return require(!QFileInfo::exists(outsidePath), QStringLiteral("Archive traversal wrote outside the destination directory"));
}

bool rejectsPrefixConfusionEntries()
{
    QTemporaryDir temporaryDirectory;
    const auto archivePath = QDir(temporaryDirectory.path()).filePath(QStringLiteral("prefix-confusion.zip"));
    const auto destination  = QDir(temporaryDirectory.path()).filePath(QStringLiteral("destination"));
    const auto siblingPath  = QDir(temporaryDirectory.path()).filePath(QStringLiteral("destination-escape/file.txt"));

    if (!require(temporaryDirectory.isValid(), QStringLiteral("Unable to create temporary test directory"))
        || !createArchive(archivePath, QStringLiteral("../destination-escape/file.txt"), QByteArrayLiteral("must not escape")))
        return false;

    mv::util::Archiver archiver;
    archiver.decompress(archivePath, destination);

    return require(!QFileInfo::exists(siblingPath), QStringLiteral("Archive entry escaped to a sibling directory"));
}

bool rejectsSymbolicLinks()
{
    QTemporaryDir temporaryDirectory;
    const auto sourceDirectory = QDir(temporaryDirectory.path()).filePath(QStringLiteral("source"));
    const auto archivePath     = QDir(temporaryDirectory.path()).filePath(QStringLiteral("symlink.zip"));
    const auto destination     = QDir(temporaryDirectory.path()).filePath(QStringLiteral("destination"));
    const auto targetPath      = QDir(sourceDirectory).filePath(QStringLiteral("target.txt"));
    const auto linkPath        = QDir(sourceDirectory).filePath(QStringLiteral("link.txt"));

    if (!require(temporaryDirectory.isValid(), QStringLiteral("Unable to create temporary test directory"))
        || !require(QDir().mkpath(sourceDirectory), QStringLiteral("Unable to create symbolic-link test directory")))
        return false;

    QFile target(targetPath);

    if (!require(target.open(QIODevice::WriteOnly), QStringLiteral("Unable to create symbolic-link target")))
        return false;

    target.write("target");
    target.close();

    if (!QFile::link(targetPath, linkPath) || !QFileInfo(linkPath).isSymLink()) {
        qWarning() << "Skipping symbolic-link archive test: symbolic links are unavailable";
        return true;
    }

    mv::util::Archiver archiver;
    archiver.compressDirectory(sourceDirectory, archivePath);

    const auto extractedLinkPath = QDir(destination).filePath(QStringLiteral("link.txt"));
    bool rejected = false;

    try {
        archiver.extractSingleFile(archivePath, QStringLiteral("link.txt"), extractedLinkPath);
    }
    catch (...) {
        rejected = true;
    }

    return require(rejected, QStringLiteral("Symbolic-link archive entry was accepted"))
        && require(!QFileInfo::exists(extractedLinkPath), QStringLiteral("Symbolic-link archive entry was materialized"));
}

bool storesFilesWhenCompressionIsDisabled()
{
    QTemporaryDir temporaryDirectory;
    const auto sourceDirectory = QDir(temporaryDirectory.path()).filePath(QStringLiteral("source"));
    const auto nestedDirectory = QDir(sourceDirectory).filePath(QStringLiteral("nested"));
    const auto sourcePath      = QDir(nestedDirectory).filePath(QStringLiteral("file.txt"));
    const auto archivePath      = QDir(temporaryDirectory.path()).filePath(QStringLiteral("stored.zip"));

    if (!require(temporaryDirectory.isValid(), QStringLiteral("Unable to create temporary test directory"))
        || !require(QDir().mkpath(nestedDirectory), QStringLiteral("Unable to create source directory")))
        return false;

    QFile source(sourcePath);
    if (!require(source.open(QIODevice::WriteOnly), QStringLiteral("Unable to create source file")))
        return false;

    source.write("content");
    source.close();

    mv::util::Archiver archiver;
    archiver.compressDirectory(sourceDirectory, archivePath, true, 0);

    QuaZip zip(archivePath);
    if (!require(zip.open(QuaZip::mdUnzip), QStringLiteral("Unable to open stored archive"))
        || !require(zip.goToFirstFile(), QStringLiteral("Stored archive does not contain the source file")))
        return false;

    QuaZipFileInfo64 info;
    do
    {
        if (!require(zip.getCurrentFileInfo(&info), QStringLiteral("Unable to inspect stored archive entry"))
            || !require(info.method == 0, QStringLiteral("Compression-disabled archive entry was not stored")))
            return false;
    }
    while (zip.goToNextFile());

    return true;
}

}

int main(int argc, char* argv[])
{
    QCoreApplication application(argc, argv);

    const auto validEntries = extractsValidEntries();
    const auto traversal    = rejectsTraversalEntries();
    const auto prefix       = rejectsPrefixConfusionEntries();
    const auto symlinks     = rejectsSymbolicLinks();
    const auto stored       = storesFilesWhenCompressionIsDisabled();

    std::fprintf(stderr, "valid=%s traversal=%s prefix=%s symlinks=%s stored=%s\n",
                 validEntries ? "pass" : "fail",
                 traversal ? "pass" : "fail",
                 prefix ? "pass" : "fail",
                 symlinks ? "pass" : "fail",
                 stored ? "pass" : "fail");

    return validEntries && traversal && prefix && symlinks && stored ? 0 : 1;
}
