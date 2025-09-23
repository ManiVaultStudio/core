// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QFileDialog>
#include <QSize>
#include <QString>
#include <QUrl>

namespace mv::gui {

/**
 * File dialog class
 * 
 * GUI class for picking files and directories for open/save (defaults with non-native dialog)
 * 
 * @author Thomas Kroes
 */
class CORE_EXPORT FileDialog : public QFileDialog
{
public:

    /**
     * Construct with pointer to \p parent widget and additional settings
     * @param parent Pointer to parent widget
     * @param caption Dialog title
     * @param directory Initial directory
     * @param filter Filter
     */
    explicit FileDialog(QWidget* parent = nullptr, const QString& caption = QString(), const QString& directory = QString(), const QString& filter = QString());

    /**
     * Get preferred size
     * @return Size in pixels
     */
    QSize sizeHint() const override {
        if (testOption(DontUseNativeDialog))
            return { 800, 600 };

        return QFileDialog::sizeHint();
    }

    /**
     * Get minimum size hint
     * @return Size in pixels
     */
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

    /**
     * Returns an existing file selected by the user, if the user presses cancel, it returns an empty file
     * @param parent Pointer to parent widget
     * @param caption Dialog title
     * @param directory Initial directory
     * @param filter File filter
     * @param selectedFilter Selected filter
     * @param options Dialog options
     * @return File path when accepted
     */
    static QString getOpenFileName(QWidget* parent = nullptr, const QString& caption = QString(), const QString& directory = QString(), const QString& filter = QString(), QString* selectedFilter = nullptr, Options options = Options());

    /**
     * Returns an existing file URL selected by the user, if the user presses cancel, it returns an empty file URL
     * @param parent Pointer to parent widget
     * @param caption Dialog title
     * @param directoryUrl Initial directory
     * @param filter File filter
     * @param selectedFilter Selected filter
     * @param options Dialog options
     * @param supportedSchemes Supported schemes
     * @return File URL when accepted
     */
    static QUrl getOpenFileUrl(QWidget* parent = nullptr, const QString& caption = QString(), const QUrl& directoryUrl = QUrl(), const QString& filter = QString(), QString* selectedFilter = nullptr, Options options = Options(), const QStringList& supportedSchemes = QStringList());

    /**
     * Returns an existing directory selected by the user, if the user presses cancel, it returns an empty directory
     * @param parent Pointer to parent widget
     * @param caption Dialog title
     * @param dir Initial directory
     * @param options Dialog options
     * @return Directory path when accepted
     */
    static QString getExistingDirectory(QWidget* parent = nullptr, const QString& caption = QString(), const QString& dir = QString(), Options options = ShowDirsOnly);

    /**
     * Returns an existing directory URL selected by the user, if the user presses cancel, it returns an empty directory URL
     * @param parent Pointer to parent widget
     * @param caption Dialog title
     * @param directoryUrl Initial directory
     * @param options Dialog options
     * @param supportedSchemes Supported schemes
     * @return Directory URL when accepted
     */
    static QUrl getExistingDirectoryUrl(QWidget* parent = nullptr, const QString& caption = QString(), const QUrl& directoryUrl = QUrl(), Options options = ShowDirsOnly, const QStringList& supportedSchemes = QStringList());
};

/**
* File open dialog class
*
* GUI class for picking a file (defaults with non-native dialog)
*
* @author Thomas Kroes
*/
class CORE_EXPORT FileOpenDialog : public FileDialog
{
public:

    /**
     * Construct with pointer to \p parent widget and additional settings
     * @param parent Pointer to parent widget
     * @param caption Dialog title
     * @param directory Initial directory
     * @param filter Filter
     */
    explicit FileOpenDialog(QWidget* parent = nullptr, const QString& caption = QString(), const QString& directory = QString(), const QString& filter = QString());
};

/**
* Directory open dialog class
*
* GUI class for picking a directory (defaults with non-native dialog)
*
* @author Thomas Kroes
*/
class CORE_EXPORT DirectoryOpenDialog : public FileDialog
{
public:

    /**
     * Construct with pointer to \p parent widget and additional settings
     * @param parent Pointer to parent widget
     * @param caption Dialog title
     * @param directory Initial directory
     * @param filter Filter
     */
    explicit DirectoryOpenDialog(QWidget* parent = nullptr, const QString& caption = QString(), const QString& directory = QString(), const QString& filter = QString());
};

/**
* File save dialog class
*
* GUI class for saving files (defaults with non-native dialog)
*
* @author Thomas Kroes
*/
class CORE_EXPORT FileSaveDialog : public FileDialog
{
public:

    /**
     * Construct with pointer to \p parent widget and additional settings
     * @param parent Pointer to parent widget
     * @param caption Dialog title
     * @param directory Initial directory
     * @param filter Filter
     */
    explicit FileSaveDialog(QWidget* parent = nullptr, const QString& caption = QString(), const QString& directory = QString(), const QString& filter = QString());
};


}