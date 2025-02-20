// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "FileDialog.h"

#ifdef _DEBUG
    #define FILE_DIALOG_VERBOSE
#endif

#define FILE_DIALOG_VERBOSE

using namespace mv::util;

namespace mv::gui {

FileDialog::FileDialog(QWidget* parent , const QString& caption, const QString& directory, const QString& filter) :
    QFileDialog(parent)
{
    setWindowTitle(caption);
    setDirectory(directory);
    setNameFilter(filter);
    setOption(DontUseNativeDialog, true);
}

QString FileDialog::getOpenFileName(QWidget* parent, const QString& caption, const QString& directory, const QString& filter, QString* selectedFilter, Options options)
{
#ifdef FILE_DIALOG_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    const auto schemes      = QStringList(QStringLiteral("file"));
    const auto selectedUrl  = getOpenFileUrl(parent, caption, QUrl::fromLocalFile(directory), filter, selectedFilter, options, schemes);

    if (selectedUrl.isLocalFile() || selectedUrl.isEmpty())
        return selectedUrl.toLocalFile();
    
    return selectedUrl.toString();
}

QUrl FileDialog::getOpenFileUrl(QWidget* parent, const QString& caption, const QUrl& directoryUrl, const QString& filter, QString* selectedFilter, Options options, const QStringList& supportedSchemes)
{
#ifdef FILE_DIALOG_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    FileOpenDialog fileOpenDialog(parent);

    fileOpenDialog.setWindowTitle(caption);
    fileOpenDialog.setDirectoryUrl(directoryUrl);
    fileOpenDialog.setNameFilter(filter);
    fileOpenDialog.setOptions(options);
	fileOpenDialog.setSupportedSchemes(supportedSchemes);

    if (selectedFilter && !selectedFilter->isEmpty())
        fileOpenDialog.selectNameFilter(*selectedFilter);

    if (fileOpenDialog.exec() == Accepted) {
        if (selectedFilter)
            *selectedFilter = fileOpenDialog.selectedNameFilter();

        return fileOpenDialog.selectedUrls().value(0);
    }

    return {};
}

QString FileDialog::getExistingDirectory(QWidget* parent, const QString& caption, const QString& dir, Options options)
{
#ifdef FILE_DIALOG_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    const auto schemes      = QStringList(QStringLiteral("file"));
    const auto selectedUrl  = getExistingDirectoryUrl(parent, caption, QUrl::fromLocalFile(dir), options, schemes);

    if (selectedUrl.isLocalFile() || selectedUrl.isEmpty())
        return selectedUrl.toLocalFile();
    
    return selectedUrl.toString();
}

QUrl FileDialog::getExistingDirectoryUrl(QWidget* parent, const QString& caption, const QUrl& directoryUrl, Options options, const QStringList& supportedSchemes)
{
#ifdef FILE_DIALOG_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    DirectoryOpenDialog directoryOpenDialog(parent);

    directoryOpenDialog.setWindowTitle(caption);
    directoryOpenDialog.setDirectoryUrl(directoryUrl);
    directoryOpenDialog.setOptions(options);
    directoryOpenDialog.setSupportedSchemes(supportedSchemes);

    if (directoryOpenDialog.exec() == Accepted)
        return directoryOpenDialog.selectedUrls().value(0);

    return {};
}

FileOpenDialog::FileOpenDialog(QWidget* parent, const QString& caption, const QString& directory, const QString& filter) :
	FileDialog(parent, caption, directory, filter)
{
#ifdef FILE_DIALOG_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    setWindowIcon(StyledIcon("folder-open"));
    setFileMode(ExistingFile);
    setAcceptMode(AcceptOpen);
}

DirectoryOpenDialog::DirectoryOpenDialog(QWidget* parent, const QString& caption, const QString& directory, const QString& filter) :
    FileDialog(parent, caption, directory, filter)
{
#ifdef FILE_DIALOG_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    setWindowIcon(StyledIcon("folder-open"));
    setFileMode(Directory);
    setAcceptMode(AcceptOpen);
}

FileSaveDialog::FileSaveDialog(QWidget* parent, const QString& caption, const QString& directory, const QString& filter) :
	FileDialog(parent, caption, directory, filter)
{
#ifdef FILE_DIALOG_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    setWindowIcon(StyledIcon("floppy-disk"));
    setAcceptMode(AcceptSave);
    setFileMode(AnyFile);
}

}
