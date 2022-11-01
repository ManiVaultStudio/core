#pragma once

#include "WidgetActionWidget.h"
#include "Serializable.h"

#include <QWidgetAction>

class QLabel;
class QMenu;

namespace hdps {

class DataHierarchyItem;
class Application;

namespace plugin {
    class Plugin;
}

namespace gui {

class WidgetActionLabel;

/**
 * Widget action class
 *
 * Base class for custom widget actions
 * 
 * @author Thomas Kroes
 */
class WidgetAction : public QWidgetAction, public Serializable
{
    Q_OBJECT

public:

    /** Describes the connection options */
    enum ConnectionFlag {
        MayPublish              = 0x00001,                                              /** Widget may publish itself (make a shared public copy) */
        MayConnect              = 0x00002,                                              /** Widget may connect to a public action */

        API                     = 0x00004,                                              /** In the context of the API */
        GUI                     = 0x00008,                                              /** In the context of the GUI */

        MayPublishViaApi        = MayPublish | API,                                     /** Widget may be published via the API */
        MayPublishViaGui        = MayPublish | GUI,                                     /** Widget may be published via the GUI */
        MayPublishViaApiAndGui  = MayPublishViaApi | MayPublishViaGui,                  /** Widget may be published via the API ánd the GUI */

        MayConnectViaApi        = MayConnect | API,                                     /** Widget may connect to a public action via the API */
        MayConnectViaGui        = MayConnect | GUI,                                     /** Widget may connect to a public action via the GUI */
        MayConnectViaApiAndGui  = MayConnectViaApi | MayConnectViaGui,                  /** Widget may connect to a public action via the API ánd the GUI */

        Default                 = MayPublishViaApiAndGui | MayConnectViaApiAndGui       /** Default allows all connection options */
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    WidgetAction(QObject* parent = nullptr);

    /** Destructor */
    ~WidgetAction();

    /**
     * Get type string
     * @return Widget action type in string format
     */
    virtual QString getTypeString() const;

    /**
     * Get parent widget action
     * @return Pointer to parent widget action (if any)
     */
    WidgetAction* getParentWidgetAction();

    /**
     * Create standard widget
     * @param parent Parent widget
     * @return Pointer to created widget
     */
    QWidget* createWidget(QWidget* parent) override;

    /**
     * Create collapsed widget
     * @param parent Parent widget
     * @return Pointer to collapsed widget
     */
    QWidget* createCollapsedWidget(QWidget* parent);

    /**
     * Create label widget
     * @param parent Parent widget
     * @param widgetFlags Label widget configuration flags
     * @return Pointer to widget
     */
    QWidget* createLabelWidget(QWidget* parent, const std::int32_t& widgetFlags = 0x00001);

    /**
     * Get the context menu for the action
     * @param parent Parent widget
     * @return Context menu
     */
    virtual QMenu* getContextMenu(QWidget* parent = nullptr);

    /**
     * Create widget for the action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags
     */
    QWidget* createWidget(QWidget* parent, const std::int32_t& widgetFlags);

    /** Get the sort index */
    std::int32_t getSortIndex() const;

    /**
     * Set the sort index
     * @param sortIndex Sorting index
     */
    void setSortIndex(const std::int32_t& sortIndex);

    /** Gets the default widget flags */
    std::int32_t getDefaultWidgetFlags() const;

    /**
     * Set the widget flags
     * @param widgetFlags Widget flags
     */
    void setDefaultWidgetFlags(const std::int32_t& widgetFlags);

public: // Highlighting

    /**
     * Highlight the action (draw the background in a different color)
     * @param highlighted Whether the action is highlighted or not
     */
    void setHighlighted(bool highlighted);

    /**
     * Determine whether the action is in a highlighted state or not
     * @return Boolean determining whether the action is in a highlighted state or not
     */
    bool isHighlighted() const;

public: // Linking

    /**
     * Get whether the action is public (visible to other actions)
     * @return Boolean indicating whether the action is public (visible to other actions)
     */
    virtual bool isPublic() const;

    /**
     * Get whether the action is published
     * @return Boolean indicating whether the action is published
     */
    virtual bool isPublished() const;

    /**
     * Get whether the action is connect to a public action
     * @return Boolean indicating whether the action is connect to a public action
     */
    virtual bool isConnected() const;

    /**
     * Publish this action so that other actions can connect to it
     * @param text Name of the published widget action
     */
    virtual void publish(const QString& name);

    /**
     * Connect this action to a public action
     * @param publicAction Pointer to public action to connect to
     */
    virtual void connectToPublicAction(WidgetAction* publicAction);

    /** Disconnect this action from a public action */
    virtual void disconnectFromPublicAction();

    /**
     * Connect a private action to this action
     * @param action Pointer to private action to connect to this action
     */
    void connectPrivateAction(WidgetAction* privateAction);

    /**
     * Disconnect a private action from this action
     * @param action Pointer to private action to disconnect from this action
     */
    void disconnectPrivateAction(WidgetAction* privateAction);

    /**
     * Get the public action (if any)
     * @return Pointer to the public action
     */
    WidgetAction* getPublicAction();

    /**
     * Get connected actions
     * @return Vector of pointers to connected actions
     */
    const QVector<WidgetAction*> getConnectedActions() const;

    /**
     * Get whether a copy of this action may published and shared via the API
     * @return Boolean determining whether a copy of this action may published and shared via the API
     */
    virtual bool mayPublishViaApi() const final {
        return _connectionFlags & ConnectionFlag::MayPublishViaApi;
    }

    /**
     * Get whether a copy of this action may published and shared via the GUI
     * @return Boolean determining whether a copy of this action may published and shared via the GUI
     */
    virtual bool mayPublishViaGui() const final {
        return _connectionFlags & ConnectionFlag::MayPublishViaGui;
    }

    /**
     * Get whether a copy of this action may published and shared via the API and/or GUI
     * @return Boolean determining whether a copy of this action may published and shared via the API and/or GUI
     */
    virtual bool mayPublish() const final {
        return mayPublishViaGui() & mayPublishViaGui();
    }

    /**
     * Get whether this action may connect to a public action via the API
     * @return Boolean determining whether this action may connect to a public action via the API
     */
    virtual bool mayConnectViaApi() const final{
        return _connectionFlags & ConnectionFlag::MayConnectViaApi;
    }

    /**
     * Get whether this action may connect to a public action via the GUI
     * @return Boolean determining whether this action may connect to a public action via the GUI
     */
    bool mayConnectViaGui() const {
        return _connectionFlags & ConnectionFlag::MayConnectViaApi;
    }

    /**
     * Get whether this action may connect to a public action via the API and/or GUI
     * @return Boolean determining whether this action may connect to a public action via the API and/or GUI
     */
    virtual bool mayConnect() const final {
        return mayConnectViaApi() & mayConnectViaGui();
    }

    /**
     * Get whether this action may connect to a public action and whether it may connect to a public action via the API
     * @return Boolean determining whether this action may connect to a public action and whether it may connect to a public action via the API
     */
    virtual bool mayPublishAndConnectViaApi() const final {
        return mayPublishViaApi() & mayConnectViaApi();
    }

    /**
     * Get whether this action may connect to a public action and whether it may connect to a public action via the GUI
     * @return Boolean determining whether this action may connect to a public action and whether it may connect to a public action via the GUI
     */
    virtual bool mayPublishAndConnectViaGui() const final {
        return mayPublishViaGui() & mayConnectViaGui();
    }

    /**
     * Get whether this action may connect to a public action and whether it may connect to a public action via the API and/or GUI
     * @return Boolean determining whether this action may connect to a public action and whether it may connect to a public action via the API and/or GUI
     */
    virtual bool mayPublishAndConnect() const final {
        return mayPublishAndConnectViaApi() & mayPublishAndConnectViaGui();
    }

    /**
     * Set flags which determine the connection behavior
     * @param connectionFlags Flags which determine the connection behavior
     */
    virtual void setConnectionFlags(std::int32_t connectionFlags) final {
        if (connectionFlags == _connectionFlags)
            return;

        _connectionFlags = connectionFlags;
    }

protected: // Linking

    /**
     * Get public copy of the action (other compatible actions can connect to it)
     * @return Pointer to public copy of the action
     */
    virtual WidgetAction* getPublicCopy() const;

public: // Settings

    /**
     * Determines whether the action can be reset to its default
     * @param recursive Check recursively
     * @return Whether the action can be reset to its default
     */
    virtual bool isResettable()
    {
        return false;
    };

    /**
     * Reset to factory default
     * @param recursive Reset to factory default recursively
     */
    virtual void reset() {};

    /**
     * Get settings path
     * @return Path of the action w.r.t. to the top-level action
     */
    QString getSettingsPath() const;

    /**
     * Set settings prefix
     * @param load Whether to restore settings after setting the prefix
     * @param settingsPrefix Settings prefix
     */
    void setSettingsPrefix(const QString& settingsPrefix, const bool& load = true);

    /**
     * Set settings prefix in the context of a plugin (the combined settings prefix will be: Plugins/PluginKind/SettingsPrefix)
     * @param load Whether to restore settings after setting the prefix
     * @param plugin Pointer to plugin context
     * @param settingsPrefix Settings prefix
     */
    void setSettingsPrefix(plugin::Plugin* plugin, const QString& settingsPrefix, const bool& load = true);

    /**
     * Get settings prefix
     * @return Settings prefix
     */
    QString getSettingsPrefix() const;

    /** Load from settings (if the settings prefix is set) */
    void loadFromSettings();

    /** Save to settings (if the settings prefix is set) */
    void saveToSettings();

    /**
     * Find child widget action of which the GUI name contains the search string
     * @param searchString The search string
     * @param recursive Whether to search recursively
     * @return Found vector of pointers to widget action(s)
     */
    QVector<WidgetAction*> findChildren(const QString& searchString, bool recursive = true) const;

protected:

    /**
     * Get widget representation of the action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    virtual QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags);

signals:

    /**
     * Signals that the highlighted state changed
     * @param highlighted Whether the action is in a highlighted state or not
     */
    void highlightedChanged(bool highlighted);

    /**
     * Signals that the published state changed
     * @param isPublished Whether the action is published or not
     */
    void isPublishedChanged(const bool& isPublished);

    /**
     * Signals that the connected state changed
     * @param isConnected Whether the action is connected or not
     */
    void isConnectedChanged(const bool& isConnected);

    /**
     * Signals that an action connected
     * @param action Action that connected
     */
    void actionConnected(const WidgetAction* action);

    /**
     * Signals that an action disconnected
     * @param action Action that disconnected
     */
    void actionDisconnected(const WidgetAction* action);

protected:
    std::int32_t                _defaultWidgetFlags;    /** Default widget flags which are used to configure newly created widget action widgets */
    std::int32_t                _sortIndex;             /** Sort index (used in the group action to sort actions) */
    std::int32_t                _connectionFlags;       /** Allowed connection options flags */
    WidgetAction*               _publicAction;          /** Public action to which this action might be connected */
    QVector<WidgetAction*>      _connectedActions;      /** Pointers to widget action that are connected to this action */
    QString                     _settingsPrefix;        /** If non-empty, the prefix is used to save the contents of the widget action to settings with the Qt settings API */
    bool                        _highlighted;           /** Whether the action is in a highlighted state or not */
};

/** List of widget actions */
using WidgetActions = QVector<WidgetAction*>;

/**
 * Print widget action to console
 * @param debug Debug
 * @param widgetAction Reference to widget action
 */
inline QDebug operator << (QDebug debug, const WidgetAction& widgetAction)
{
    debug.noquote().nospace() << widgetAction.text();

    return debug.space();
}

/**
 * Print widget action to console
 * @param debug Debug
 * @param widgetAction Pointer to widget action
 */
inline QDebug operator << (QDebug debug, WidgetAction* widgetAction)
{
    debug.noquote().nospace() << widgetAction->text();

    return debug.space();
}

}
}
