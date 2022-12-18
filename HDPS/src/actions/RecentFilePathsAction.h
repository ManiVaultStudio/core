#pragma once

#include "WidgetAction.h"
#include "TriggerAction.h"

#include <QStandardItemModel>

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

public:

    /** Widget class for recent file paths action */
    class Widget : public WidgetActionWidget {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param recentFilePathsAction Pointer to recent file paths action
         * @param widgetFlags Widget flags
         */
        Widget(QWidget* parent, RecentFilePathsAction* recentFilePathsAction, const std::int32_t& widgetFlags);

    protected:
        RecentFilePathsAction*  _recentFilePathsAction;     /** Pointer to recent file paths action */

        friend class RecentFilePathsAction;
    };

public:

    /**
     * Get type string
     * @return Widget action type in string format
     */
    QString getTypeString() const override;

protected:

    /**
     * Get widget representation of the recent file paths action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this, widgetFlags);
    }

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param settingsKey Settings key where the recent file paths will be stored
     * @param fileType Type of file e.g. project or workspace
     * @param icon Icon in menu
     */
    RecentFilePathsAction(QObject* parent, const QString& settingsKey = "", const QString& fileType = "", const QIcon& icon = QIcon());

    /**
     * Initializes the action
     * @param parent Pointer to parent object
     * @param settingsKey Settings key where the recent file paths will be stored
     * @param fileType Type of file e.g. project or workspace
     * @param icon Icon in menu
     */
    void initialize(const QString& settingsKey, const QString& fileType, const QIcon& icon);

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
     * Get icon
     * @return Icon for the recent file type
     */
    QIcon getIcon() const;

    /**
     * Get recent file paths menu
     * @return Pointer to menu
     */
    QMenu* getMenu();

signals:

    /** Signals that recent \p filePath is triggered */
    void triggered(const QString& filePath);

private:
    QString                 _settingsKey;   /** Settings key where the recent file paths will be stored */
    QString                 _fileType;      /** Recent file type */
    QIcon                   _icon;          /** Icon for the recent file type */
    Model                   _model;         /** Model for storing recent file paths */
};

}
