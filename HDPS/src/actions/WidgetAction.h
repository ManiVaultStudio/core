#pragma once

#include "WidgetActionWidget.h"

#include "util/Serializable.h"

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
class WidgetAction : public QWidgetAction, public util::Serializable
{
    Q_OBJECT

public:

    /** Describes the configuration options */
    enum class ConfigurationFlag {
        VisibleInMenu       = 0x00001,      /** Whether the action may show itself in (context) menus */
        InternalUseOnly     = 0x00002,      /** Action is only for internal use, it is not part of the graphical user interface */
        NoLabelInGroup      = 0x00004,      /** Action will not have a label when it is displayed in a group */
        AlwaysCollapsed     = 0x00008,      /** Action will be always collapsed, no matter the circumstances */

        Default = 0
    };

    ///** Describes the connection context options */
    enum ConnectionContextFlag {
        Api = 0x00008,      /** In the context of the API */
        Gui = 0x00010,      /** In the context of the GUI */

        ApiAndGui = Api | Gui
    };

    /** Describes the connection permission options */
    enum class ConnectionPermissionFlag {
        None                    = 0x00000,                              /** Widget may not published nor connect nor disconnect via API and GUI */

        PublishViaApi           = 0x00001,                              /** Widget may be published via the API */
        PublishViaGui           = 0x00002,                              /** Widget may be published via the GUI */
        PublishViaApiAndGui     = PublishViaApi | PublishViaGui,        /** Widget may be published via the API and the GUI */

        ConnectViaApi           = 0x00008,                              /** Widget may connect to a public action via the API */
        ConnectViaGui           = 0x00010,                              /** Widget may connect to a public action via the GUI */
        ConnectViaApiAndGui     = ConnectViaApi | ConnectViaGui,        /** Widget may connect to a public action via the API and the GUI */

        DisconnectViaApi        = 0x00040,                              /** Widget may disconnect from a public action via the API */
        DisconnectViaGui        = 0x00080,                              /** Widget may disconnect from a public action via the GUI */
        DisconnectViaApiAndGui  = DisconnectViaApi | DisconnectViaGui,  /** Widget may disconnect from a public action via the API and the GUI */

        All     = PublishViaApiAndGui | ConnectViaApiAndGui | DisconnectViaApiAndGui,
        Default = All
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
    virtual QWidget* createWidget(QWidget* parent) override final;

    /**
     * Create collapsed widget
     * @param parent Parent widget
     * @return Pointer to collapsed widget
     */
    virtual QWidget* createCollapsedWidget(QWidget* parent) const final;

    /**
     * Create label widget
     * @param parent Parent widget
     * @param widgetFlags Label widget configuration flags
     * @return Pointer to widget
     */
    virtual QWidget* createLabelWidget(QWidget* parent, const std::int32_t& widgetFlags = 0x00001) const final;

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

    /**
     * Get stretch
     * @return The stretch factor
     */
    std::int32_t getStretch() const;

    /**
     * Set stretch to \p stretch
     * @param stretch Stretch factor
     */
    void setStretch(const std::int32_t& stretch);

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
     * Get whether this action is a public (shared) action or not
     * @return Boolean determining whether this action is a public (shared) action or not
     */
    virtual bool isPublic() const;

    /**
     * Get whether the action is published
     * @return Boolean indicating whether the action is published
     */
    virtual bool isPublished() const final;

    /**
     * Get whether the action is connect to a public action
     * @return Boolean indicating whether the action is connect to a public action
     */
    virtual bool isConnected() const final;

    /**
     * Publish this action so that other actions can connect to it
     * @param text Name of the published widget action (if empty, a configuration dialog will popup)
     */
    virtual void publish(const QString& name = "");

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
     * Get whether a copy of this action may published and shared, depending on the \p connectionContextFlags
     * @param connectionContextFlags The context from which the action will be published (API and/or GUI)
     * @return Boolean determining whether a copy of this action may published and shared, depending on the \p connectionContextFlags
     */
    virtual bool mayPublish(ConnectionContextFlag connectionContextFlags) const final;

    /**
     * Get whether this action may connect to a public action, depending on the \p connectionContextFlags
     * @param connectionContextFlags The context from which the connection will be made (API and/or GUI)
     * @return Boolean determining whether this action may connect to a public action, depending on the \p connectionContextFlags
     */
    virtual bool mayConnect(ConnectionContextFlag connectionContextFlags) const final;

    /**
     * Get whether this action may disconnect from a public action, depending on the \p connectionContextFlags
     * @param connectionContextFlags The context from which the disconnection will be initiated (API and/or GUI)
     * @return Boolean determining whether this action may disconnect from a public action, depending on the \p connectionContextFlags
     */
    virtual bool mayDisconnect(ConnectionContextFlag connectionContextFlags) const final;

    /**
     * Get connection permission flags
     * @return Connection permission flags
     */
    virtual std::int32_t getConnectionPermissions() const final;

    /**
     * Check whether \p connectionPermissionsFlag is set or not
     * @param connectionPermissionsFlag Connection permissions flag
     * @return Boolean determining whether \p connectionPermissionsFlag is set or not
     */
    virtual bool isConnectionPermissionFlagSet(ConnectionPermissionFlag connectionPermissionsFlag) final;

    /**
     * Set connection permissions flag
     * @param connectionPermissionsFlag Connection permissions flag to set
     * @param unset Whether to unset the connection permissions flag
     * @param recursive Whether to recursively set child connection permissions
     */
    virtual void setConnectionPermissionsFlag(ConnectionPermissionFlag connectionPermissionsFlag, bool unset = false, bool recursive = false) final;

    /**
     * Set connection permissions
     * @param connectionPermissions Connection permissions value
     * @param recursive Whether to recursively set child connection permissions
     */
    virtual void setConnectionPermissions(std::int32_t connectionPermissions, bool recursive = false) final;

    /**
     * Reset connection permissions to none
     * @param recursive Whether to recursively set child connection permissions
     */
    virtual void setConnectionPermissionsToNone(bool recursive = false) final;

protected: // Linking

    /**
     * Get public copy of the action (other compatible actions can connect to it)
     * @return Pointer to public copy of the action
     */
    virtual WidgetAction* getPublicCopy() const;

public: // Settings

    /**
     * Determines whether the action can be reset to its default
     * @return Whether the action can be reset to its default
     */
    virtual bool isResettable();;

    /** Reset to default */
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
    
public: // Popups

    /**
     * Get size hint of popups (in case of collapsed actions)
     * @return Popup size hint
     */
    QSize getPopupSizeHint() const;

    /**
     * Set size hint of popups (in case of collapsed actions)
     * @param popupSizeHint Popup size hint
     */
    void setPopupSizeHint(const QSize& popupSizeHint);

public: // Configuration flags

    /**
     * Get configuration
     * @return Configuration
     */
    virtual std::int32_t getConfiguration() const final;

    /**
     * Check whether \p configurationFlag is set or not
     * @param configurationFlag Configuration flag
     * @return Boolean determining whether \p configurationFlag is set or not
     */
    virtual bool isConfigurationFlagSet(ConfigurationFlag configurationFlag) const final;

    /**
     * Set configuration flag
     * @param configurationFlag Configuration flag to set
     * @param unset Whether to unset the \p configurationFlag flag
     * @param recursive Whether to recursively set child child configuration flag
     */
    virtual void setConfigurationFlag(ConfigurationFlag configurationFlag, bool unset = false, bool recursive = false) final;

    /**
     * Set configuration
     * @param configuration Configuration value
     * @param recursive Whether to recursively set child child configuration flag
     */
    virtual void setConfiguration(std::int32_t configuration, bool recursive = false) final;

protected:

    /**
     * Get widget representation of the action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    virtual QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags);

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

public: // State caching

    /**
     * Cache the state of a widget action under \p name in the action itself (for global presets use the presets action)
     * @param name Name to use for the cached widget action state
     */
    void cacheState(const QString& name = "cache");

    /**
     * Restore the state of under \p name
     * @param name Name of the cached widget action state to restore
     * @param remove Whether to remove the cache
     */
    void restoreState(const QString& name = "cache", bool remove = true);

public:

    /**
     * Get child actions
     * @return Vector of pointers to child actions
     */
    QVector<WidgetAction*> getChildActions();

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

    /**
     * Signals that the connection permissions changed
     * @param connectionPermissions New connection permissions
     */
    void connectionPermissionsChanged(std::int32_t connectionPermissions);

    /**
     * Signals that \p configurationFlag is \p set
     * @param configurationFlag Toggled configuration flag
     * @param set Whether the flag was set or unset
     */
    void configurationFlagToggled(const ConfigurationFlag& configurationFlag, bool set);

    /**
     * Signals that the configuration changed
     * @param configuration New configuration
     */
    void configurationChanged(std::int32_t configuration);

private:
    std::int32_t                _defaultWidgetFlags;        /** Default widget flags which are used to configure newly created widget action widgets */
    std::int32_t                _sortIndex;                 /** Sort index (used in the group action to sort actions) */
    std::int32_t                _stretch;                   /** Stretch factor */
    std::int32_t                _connectionPermissions;     /** Allowed connection options flags */
    bool                        _isPublic;                  /** Determines whether this action is a public (shared) action or not */
    WidgetAction*               _publicAction;              /** Public action to which this action might be connected */
    QVector<WidgetAction*>      _connectedActions;          /** Pointers to widget action that are connected to this action */
    QString                     _settingsPrefix;            /** If non-empty, the prefix is used to save the contents of the widget action to settings with the Qt settings API */
    bool                        _highlighted;               /** Whether the action is in a highlighted state or not */
    QSize                       _popupSizeHint;             /** Size hint of the popup */
    std::int32_t                _configuration;             /** Configuration flags */
    QMap<QString, QVariant>     _cachedStates;              /** Maps cache name to state */
};

using WidgetActions = QVector<WidgetAction*>;
using ConstWidgetActions = QVector<const WidgetAction*>;

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
