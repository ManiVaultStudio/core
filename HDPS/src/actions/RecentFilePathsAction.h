#pragma once

#include "WidgetAction.h"
#include "TriggerAction.h"

#include "widgets/HierarchyWidget.h"

#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QDialog>

namespace hdps::gui {

/**
 * Recent file paths action class
 *
 * Action class for storing and editing recent file paths and saving them to settings.
 *
 * @author Thomas Kroes
 */
class RecentFilePathsAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Standard item model loading and manipulating the recent file paths */
    class Model final : public QStandardItemModel {
    protected:

        /**
         * Constructor
         * @param recentFilePathsAction Pointer to owning recent file paths action
         */
        Model(RecentFilePathsAction* recentFilePathsAction);

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

    private:
        RecentFilePathsAction*  _recentFilePathsAction;     /** Pointer to owning recent file paths action */
        QList<TriggerAction*>   _actions;                   /** Menu trigger actions */

        friend class RecentFilePathsAction;
    };

    class FilterModel : public QSortFilterProxyModel
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
    class Dialog : public QDialog
    {
    protected:

        /**
         * Construct a dialog with owning \p recentFilePathsAction
         * @param recentFilePathsAction Pointer to recent file paths action
         */
        Dialog(RecentFilePathsAction* recentFilePathsAction);

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

        friend class RecentFilePathsAction;
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param settingsKey Settings key where the recent file paths will be stored
     * @param fileType Type of file e.g. project or workspace
     * @param shortcutPrefix Prefix of the shortcut
     * @param icon Icon in menu
     */
    RecentFilePathsAction(QObject* parent, const QString& settingsKey = "", const QString& fileType = "", const QString& shortcutPrefix = "", const QIcon& icon = QIcon());

    /**
     * Get type string
     * @return Widget action type in string format
     */
    QString getTypeString() const override;

    /**
     * Initializes the action
     * @param parent Pointer to parent object
     * @param settingsKey Settings key where the recent file paths will be stored
     * @param fileType Type of file e.g. project or workspace
     * @param shortcutPrefix Prefix of the shortcut
     * @param icon Icon in menu
     */
    void initialize(const QString& settingsKey, const QString& fileType, const QString& shortcutPrefix, const QIcon& icon);

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
     * Get icon
     * @return Icon for the recent file type
     */
    QIcon getIcon() const;

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

signals:

    /** Signals that recent \p filePath is triggered */
    void triggered(const QString& filePath);

private:
    QString         _settingsKey;       /** Settings key where the recent file paths will be stored */
    QString         _fileType;          /** Recent file type */
    QString         _shortcutPrefix;    /** Shortcut prefix (if empty, no shortcut is created) */
    QIcon           _icon;              /** Icon for the recent file type */
    Model           _model;             /** Model for storing recent file paths */
    FilterModel     _filterModel;       /** Sort/filter model */
    TriggerAction   _editAction;        /** Action which triggers a dialog in which the recent file paths can be edited */
};

}
