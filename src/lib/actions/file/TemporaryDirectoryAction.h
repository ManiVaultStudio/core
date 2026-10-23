// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/DirectoryPickerAction.h"
#include "actions/HorizontalGroupAction.h"
#include "actions/TriggerAction.h"

namespace mv::gui
{

/**
 * Temporary directory action class
 *
 * Action for selecting a temporary directory and restoring its default value.
 * The selected directory is serialized as the action's Value so it remains
 * compatible with the existing application temporary-directory setting.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT TemporaryDirectoryAction final : public HorizontalGroupAction
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param directory Current directory
     * @param defaultDirectory Directory used by the reset action
     */
    Q_INVOKABLE TemporaryDirectoryAction(QObject* parent, const QString& title, const QString& directory = QString(), const QString& defaultDirectory = QString());

    /** Get the selected directory */
    QString getDirectory() const;

    /** Set the selected directory */
    void setDirectory(const QString& directory);

    /** Get the default directory used by the reset action */
    QString getDefaultDirectory() const;

    /** Reset the selected directory to its default */
    void reset();

public: // Action getters

    DirectoryPickerAction& getDirectoryPickerAction() { return _directoryPickerAction; }
    TriggerAction& getResetAction() { return _resetAction; }

public: // Serialization

    /** Load the selected directory from a variant map */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /** Save the selected directory to a variant map */
    QVariantMap toVariantMap() const override;

signals:

    /** Signals that the selected directory changed */
    void directoryChanged(const QString& directory);

private:
    DirectoryPickerAction _directoryPickerAction;
    TriggerAction _resetAction;
    QString _defaultDirectory;
};

}

Q_DECLARE_METATYPE(mv::gui::TemporaryDirectoryAction)

inline const auto temporaryDirectoryActionMetaTypeId = qRegisterMetaType<mv::gui::TemporaryDirectoryAction*>("mv::gui::TemporaryDirectoryAction");
