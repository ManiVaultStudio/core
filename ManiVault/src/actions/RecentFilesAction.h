// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetAction.h"
#include "TriggerAction.h"

#include "widgets/HierarchyWidget.h"

#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QDialog>

namespace mv::gui {

/** Recent file utility class (contains file path and date time) */
class CORE_EXPORT RecentFile final {
public:

    /**
     * Construct with \p filePath and \p dateTime
     * @param filePath File path of the recent file
     * @param dateTime Date and time the recent file was last opened
     */
    RecentFile(const QString filePath, const QDateTime dateTime) :
        _filePath(filePath),
        _dateTime(dateTime)
    {
    }

    /**
     * Get file path
     * @return File path of the recent file
     */
    QString getFilePath() const {
        return _filePath;
    }

    /**
     * Get date and time
     * @return Date and time the recent file was last opened
     */
    QDateTime getDateTime() const {
        return _dateTime;
    }

private:
    QString       _filePath;  /** File path of the recent file */
    QDateTime     _dateTime;  /** Date and time the recent file was last opened */
};

using RecentFiles = QList<RecentFile>;

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

    /** Standard item model loading and manipulating the recent file paths */
    class CORE_EXPORT Model final : public QStandardItemModel {
    public:

        /** Model columns */
        enum class Column {
            FilePath,       /** Location of the recent file */
            DateTime        /** Date and time when the file was opened */
        };

        /** Column name and tooltip */
        static QMap<Column, QPair<QString, QString>> columnInfo;

    protected:

        /**
         * Constructor
         * @param recentFilePathsAction Pointer to owning recent file paths action
         */
        Model(RecentFilesAction* recentFilePathsAction);

        /** Load recent file paths from settings */
        void loadFromSettings();

        /**
         * Add recent \p filePath
         * @param filePath Recent file path to add
         */
        void addRecentFilePath(const QString& filePath);

        /**
         * Remove recent \p filePath
         * @param filePath Recent file path to remove
         */
        void removeRecentFilePath(const QString& filePath);

        /**
         * Get trigger actions for triggering the recent file path
         * @return List of trigger actions for triggering the recent file path
         */
        QList<TriggerAction*> getActions();

        /**
         * Get recent file paths
         * @return File paths as string list
         */
        QStringList getRecentFilePaths() const;

    private:
        RecentFilesAction*      _recentFilesAction; /** Pointer to owning recent file paths action */
        QList<TriggerAction*>   _actions;           /** Menu trigger actions */

        friend class RecentFilesAction;
    };

    class CORE_EXPORT FilterModel : public QSortFilterProxyModel
    {
    public:

        /**
         * Construct the filter model with \p parent
         * @param parent Pointer to parent object
        */
        FilterModel(QObject* parent = nullptr);

        /**
         * Returns whether \p row with \p parent is filtered out (false) or in (true)
         * @param row Row index
         * @param parent Parent index
         * @return Boolean indicating whether the item is filtered in or out
         */
        bool filterAcceptsRow(int row, const QModelIndex& parent) const override;

        /**
         * Compares two model indices plugin \p lhs with \p rhs
         * @param lhs Left-hand model index
         * @param rhs Right-hand model index
         */
        bool lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const override;
    };

    /** Dialog for editing recent file paths */
    class CORE_EXPORT Dialog : public QDialog
    {
    protected:

        /**
         * Construct a dialog with owning \p recentFilePathsAction
         * @param recentFilePathsAction Pointer to recent file paths action
         */
        Dialog(RecentFilesAction* recentFilePathsAction);

        /** Get preferred size */
        QSize sizeHint() const override {
            return QSize(640, 480);
        }

        /** Get minimum size hint*/
        QSize minimumSizeHint() const override {
            return sizeHint();
        }

    private:
        HierarchyWidget  _hierarchyWidget;      /** Widget for displaying the loaded plugins */
        TriggerAction    _removeAction;         /** Action for removing one, or more, recent file paths */
        TriggerAction    _okAction;             /** Action for exiting the dialog */

        friend class RecentFilesAction;
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param settingsKey Settings key where the recent file paths are stored
     * @param fileType Type of file e.g. project or workspace
     * @param shortcutPrefix Prefix of the shortcut
     * @param icon Icon in menu
     */
    Q_INVOKABLE RecentFilesAction(QObject* parent, const QString& settingsKey = "", const QString& fileType = "", const QString& shortcutPrefix = "", const QIcon& icon = QIcon());

    /**
     * Initializes the action
     * @param settingsKey Settings key where the recent file paths are stored
     * @param fileType Type of file e.g. project or workspace
     * @param shortcutPrefix Prefix of the shortcut
     */
    void initialize(const QString& settingsKey, const QString& fileType, const QString& shortcutPrefix);

    /**
     * Add recent \p filePath
     * @param filePath Recent file path
     */
    void addRecentFilePath(const QString& filePath);

    /**
     * Get settings key
     * @return Settings key where the recent file paths will be stored
     */
    QString getSettingsKey() const;

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
     * Get model
     * @return Reference to model
     */
    Model& getModel();

    /**
     * Get filter model
     * @return Reference to filter model
     */
    const FilterModel& getFilterModel() const;

    /**
     * Get edit action
     * @return Action which triggers a dialog in which the recent file paths can be edited
     */
    TriggerAction& getEditAction();

    /**
     * Get recent file paths menu
     * @return Pointer to menu
     */
    QMenu* getMenu();

    /** Grab latest recent file paths from settings and update model */
    void updateRecentFilePaths();

    /**
     * Get recent file paths
     * @return File paths as string list
     */
    QStringList getRecentFilePaths() const;

    /**
     * Get recent files
     * @return Recent files list
     */
    RecentFiles getRecentFiles() const;
    
signals:

    /** Signals that recent \p filePath is triggered */
    void triggered(const QString& filePath);

    /**
     * Signals that recent files changed
     * @param recentFiles Recent files which changed
     */
    void recentFilesChanged(const RecentFiles& recentFiles);

private:
    QString         _settingsKey;       /** Settings key where the recent file paths will be stored */
    QString         _fileType;          /** Recent file type */
    QString         _shortcutPrefix;    /** Shortcut prefix (if empty, no shortcut is created) */
    Model           _model;             /** Model for storing recent file paths */
    TriggerAction   _editAction;        /** Action which triggers a dialog in which the recent file paths can be edited */
};

}

Q_DECLARE_METATYPE(mv::gui::RecentFilesAction)

inline const auto recentFilesActionMetaTypeId = qRegisterMetaType<mv::gui::RecentFilesAction*>("RecentFilesAction");
