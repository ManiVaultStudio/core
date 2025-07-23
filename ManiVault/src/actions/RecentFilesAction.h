// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetAction.h"
#include "TriggerAction.h"

#include "widgets/HierarchyWidget.h"

#include "models/RecentFilesListModel.h"
#include "models/RecentFilesFilterModel.h"

#include <QDialog>

namespace mv::gui {

/**
 * Recent files action class
 *
 * Action class for storing and editing recent file paths and saving them to settings.
 *
 * Note: This action is developed for internal use only
 * 
 * @author Thomas Kroes
 */
class CORE_EXPORT RecentFilesAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Dialog for editing recent file paths */
    class CORE_EXPORT Dialog : public QDialog
    {
    protected:

        /**
         * Construct a dialog with owning \p recentFilePathsAction
         * @param recentFilesAction Pointer to recent file paths action
         */
        Dialog(RecentFilesAction* recentFilesAction);

        /** Get preferred size */
        QSize sizeHint() const override {
            return QSize(640, 480);
        }

        /** Get minimum size hint*/
        QSize minimumSizeHint() const override {
            return sizeHint();
        }

    private:
        RecentFilesFilterModel  _filterModel;       /** Filter model for filtering the recent file paths */
        HierarchyWidget         _hierarchyWidget;   /** Widget for displaying the loaded plugins */
        TriggerAction           _removeAction;      /** Action for removing one, or more, recent file paths */
        TriggerAction           _okAction;          /** Action for exiting the dialog */
        
        friend class RecentFilesAction;
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param fileType Type of file e.g. project or workspace
     * @param shortcutPrefix Prefix of the shortcut
     * @param icon Icon in menu
     */
    Q_INVOKABLE RecentFilesAction(QObject* parent, const QString& fileType, const QString& shortcutPrefix = "", const QIcon& icon = QIcon());

    /**
     * Initializes the action
     * @param fileType Type of file e.g. project or workspace
     * @param shortcutPrefix Prefix of the shortcut
     */
    void initialize(const QString& fileType, const QString& shortcutPrefix);

    /**
     * Add recent \p filePath
     * @param filePath Recent file path
     */
    void addRecentFilePath(const QString& filePath);

    /**
     * Get file type
     * @return Recent file type
     */
    QString getFileType() const;

    /**
     * Get shortcut prefix
     * @return Shortcut prefix (if empty, no shortcut is created)
     */
    QString getShortcutPrefix() const;

    /**
     * Get list model
     * @return Const reference to list model
     */
    const RecentFilesListModel& getModel() const;

    /**
     * Get edit action
     * @return Action which triggers a dialog in which the recent file paths can be edited
     */
    TriggerAction& getEditAction();

    /**
     * Get recent file paths menu
     * @param parent Parent widget for the menu
     * @return Pointer to menu
     */
    QMenu* getMenu(QWidget* parent);

    /** Grab latest recent file paths from settings and update model */
    void updateRecentFilePaths();

    /**
     * Get recent file paths
     * @return File paths as string list
     */
    QStringList getRecentFilePaths() const;

    /**
     * Get recent files
     * @return List of recent file objects
     */
    util::RecentFiles getRecentFiles() const;

signals:

	/** Signals that recent \p filePath is triggered */
    void triggered(const QString& filePath);

protected:

    /**
     * Get list model
     * @return Reference to list model
     */
    RecentFilesListModel& getModel();

private:
    QString                 _fileType;          /** Recent file type */
    QString                 _shortcutPrefix;    /** Shortcut prefix (if empty, no shortcut is created) */
    RecentFilesListModel    _listModel;         /** Model for storing recent file paths in list format */
    TriggerAction           _editAction;        /** Action which triggers a dialog in which the recent file paths can be edited */
};

}

Q_DECLARE_METATYPE(mv::gui::RecentFilesAction)

inline const auto recentFilesActionMetaTypeId = qRegisterMetaType<mv::gui::RecentFilesAction*>("RecentFilesAction");
