// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetAction.h"
#include "StringAction.h"
#include "TriggerAction.h"

#include <QLineEdit>
#include <QString>
#include <QFileSystemModel>
#include <QCompleter>

class QWidget;

namespace mv::gui {

/**
 * Directory picker action class
 *
 * Action class for picking a directory
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT DirectoryPickerAction : public WidgetAction
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
     * Get widget representation of the directory picker action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param directory Directory
     */
    explicit DirectoryPickerAction(QObject* parent, const QString& title, const QString& directory = QString());

    /**
     * Get the current directory
     * @return Current directory
     */
    QString getDirectory() const;

    /**
     * Set the current directory
     * @param directory Current directory
     */
    void setDirectory(const QString& directory);

    /**
     * Get placeholder text
     * @return Placeholder string
     */
    QString getPlaceholderString() const;

    /**
     * Set placeholder text
     * @param placeholderText Placeholder text
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

public: // Serialization

    /**
     * Load widget action from variant map
     * @param Variant map representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant map
     * @return Variant map representation of the widget action
     */
    QVariantMap toVariantMap() const override;

signals:

    /**
     * Signals that the directory changed
     * @param directory Directory that changed
     */
    void directoryChanged(const QString& directory);

    /**
     * Signals that the placeholder string changed
     * @param placeholderString Placeholder string that changed
     */
    void placeholderStringChanged(const QString& placeholderString);

protected:
    QFileSystemModel    _dirModel;              /** Directory model */
    QCompleter          _completer;             /** Completer */
    StringAction        _directoryAction;       /** Directory action */
    TriggerAction       _pickAction;            /** Pick directory action */
    bool                _useNativeDialog;       /** Whether to use native or Qt file dialog */
};

}

Q_DECLARE_METATYPE(mv::gui::DirectoryPickerAction)

inline const auto directoryPickerActionMetaTypeId = qRegisterMetaType<mv::gui::DirectoryPickerAction*>("mv::gui::DirectoryPickerAction");
