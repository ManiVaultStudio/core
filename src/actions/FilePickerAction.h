// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "StringAction.h"
#include "TriggerAction.h"
#include "WidgetAction.h"

#include <QCompleter>
#include <QFileSystemModel>
#include <QLineEdit>
#include <QPointer>
#include <QString>

class QWidget;

namespace mv::gui {

/**
 * File picker action class
 *
 * Action class for picking a file
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT FilePickerAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Describes the widget configurations */
    enum WidgetFlag {
        LineEdit    = 0x00001,      /** Widget includes a line edit */
        PushButton  = 0x00002,      /** There is a button to pick a directory */

        Default = LineEdit | PushButton
    };

protected:

    /**
     * Get widget representation of the file picker action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param filePath File path
     * @param populateCompleter Whether to create a QCompleter for _filePathAction
     */
    Q_INVOKABLE FilePickerAction(QObject* parent, const QString& title, const QString& filePath = QString(), bool populateCompleter = true);
    
    /**
     * Get the current file path
     * @return Current file path
     */
    QString getFilePath() const;

    /**
     * Get the application-relative file path (if it exists)
     * @return File path relative to the application directory when the file was picked, if the picked file path was a sub-path of the application directory
     */
    QString getApplicationRelativeFilePath() const;

    /**
     * Set the file path
     * @param filePath Current file path
     */
    void setFilePath(const QString& filePath);

    /**
     * Get the name filters
     * @return Name filters
     */
    QStringList getNameFilters() const;

    /**
     * Set the name filters to \p nameFilters
     * @param nameFilters Name filters
     */
    void setNameFilters(const QStringList& nameFilters);

    /**
     * Get the default suffix
     * @return Default suffix
     */
    QString getDefaultSuffix() const;

    /**
     * Set the default suffix to \p defaultSuffix
     * @param defaultSuffix Default suffix
     */
    void setDefaultSuffix(const QString& defaultSuffix);

    /**
     * Get the file type
     * @return File type
     */
    QString getFileType() const;

    /**
     * Set file type to \p fileType
     * @param fileType File type
     */
    void setFileType(const QString& fileType);

    /**
     * Get placeholder text
     * @return Placeholder string
     */
    QString getPlaceholderString() const;

    /**
     * Set placeholder text
     * @param placeholderString Placeholder text
     */
    void setPlaceHolderString(const QString& placeholderString);

    /**
     * Get directory name
     * @return Directory name
     */
    QString getDirectoryName() const;

    /**
     * Set whether to use native or Qt file dialog
     * @param useNativeDialog Whether to use native or Qt file dialog
     */
    void setUseNativeFileDialog(bool useNativeDialog);

    /**
     * Get whether to use native or Qt file dialog
     * @return Bool whether to use native or Qt file dialog
     */
    bool getUseNativeFileDialog() const;

    /**
     * Get whether the directory is valid or not
     * @return Boolean indication whether the directory is valid or not
     */
    bool isValid() const;

    /**
     * Get the application directory
     * @return Application directory
     */
    QString getApplicationDirectory() const;


public: // Serialization

    /**
     * Load file picker action from variant
     * @param variantMap Variant representation of the file picker action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save file picker action to variant
     * @return Variant representation of the file picker action
     */
    QVariantMap toVariantMap() const override;

public: // Action getters

    StringAction& getFilePathAction() { return _filePathAction; }
    TriggerAction& getPickAction() { return _pickAction; }

signals:

    /**
     * Signals that the file path changed to \p filePath
     * @param filePath New file path
     */
    void filePathChanged(const QString& filePath);

    /**
     * Signals that the placeholder string changed
     * @param placeholderString Placeholder string that changed
     */
    void placeholderStringChanged(const QString& placeholderString);

    /**
     * Signals that the file type has changed to \p fileType
     * @param fileType File type
     */
    void fileTypeChanged(const QString& fileType);

private:
    QPointer<QFileSystemModel>  _dirModel;                      /** Directory model */
    QPointer<QCompleter>        _completer;                     /** Completer */
    StringAction                _filePathAction;                /** File path action */
    TriggerAction               _pickAction;                    /** Pick file action */
    QStringList                 _nameFilters;                   /** File type filters */
    QString                     _defaultSuffix;                 /** Default suffix */
    QString                     _fileType;                      /** File type (e.g. image and project)*/
    bool                        _useNativeDialog;               /** Whether to use native or Qt file dialog */
    QString                     _applicationRelativeFilePath;   /** Path of the file, relative to the application directory */
};

}

Q_DECLARE_METATYPE(mv::gui::FilePickerAction)

inline const auto filePickerActionMetaTypeId = qRegisterMetaType<mv::gui::FilePickerAction*>("mv::gui::FilePickerAction");
