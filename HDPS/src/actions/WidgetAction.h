// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetActionWidget.h"

#include "util/Serializable.h"

#include <QWidgetAction>
#include <QPointer>

class QLabel;
class QMenu;

namespace mv {

class DataHierarchyItem;
class Application;
class AbstractActionsManager;

namespace plugin {
    class Plugin;
}

namespace gui {

class WidgetActionLabel;
class WidgetAction;

using WidgetActions = QVector<WidgetAction*>;
using ConstWidgetActions = QVector<const WidgetAction*>;

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

    /** Describes the widget action scope of use */
    enum class Scope {
        Private,    /** Action is in the private pool */
        Public      /** Action is in the public pool and eligible for connections */
    };

    /** Map scope enum to scope name */
    static QMap<Scope, QString> scopeNames;

    /** Describes the highlight options */
    enum class HighlightOption {
        None,       /** Action is not highlighted */
        Moderate,   /** Action is moderately highlighted */
        Strong      /** Action is strongly highlighted */
    };

    /** Describes the configuration options */
    enum class ConfigurationFlag {
        VisibleInMenu           = 0x00001,      /** Whether the action may show itself in (context) menus */
        InternalUseOnly         = 0x00002,      /** Action is only for internal use, it is not part of the graphical user interface */
        NoLabelInGroup          = 0x00004,      /** Action will not have a label when it is displayed in a group */
        ForceCollapsedInGroup   = 0x00008,      /** Action will be collapsed in a horizontal group (or toolbar), no matter the circumstances */
        ForceExpandedInGroup    = 0x00010,      /** Action will be expanded in a horizontal group (or toolbar), no matter the circumstances */

        User                    = 0x00100,      /** And beyond for custom configuration flags */

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

        ForceNone               = 0x00100,                              /** Disables all connection options (API and GUI), regardless of other connection permission flags */

        All     = PublishViaApiAndGui | ConnectViaApiAndGui | DisconnectViaApiAndGui,
        Default = All
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Widget action title and serialization name
     */
    WidgetAction(QObject* parent, const QString& title);

    /** Destructor */
    ~WidgetAction();

public: // Hierarchy queries

    /**
     * Get parent widget action
     * @return Pointer to parent widget action (if any)
     */
    virtual WidgetAction* getParentAction() const final;

    /**
     * Get parent widget actions
     * @return Pointer to parent widget actions
     */
    virtual WidgetActions getParentActions() const final;

    /**
     * Get child actions
     * @return Vector of pointers to child actions
     */
    virtual WidgetActions getChildren() const final;

    /**
     * Establish whether this action is positioned at the top of the hierarchy
     * @return Boolean determining whether this action is positioned at the top of the hierarchy
     */
    virtual bool isRoot() const final;

    /**
     * Establish whether this action is positioned at the bottom of the hierarchy
     * @return Boolean determining whether this action is positioned at the bottom of the hierarchy
     */
    virtual bool isLeaf() const final;

public: // Widgets
     
    /**
     * Create standard widget
     * @param parent Parent widget
     * @return Pointer to created widget
     */
    virtual QWidget* createWidget(QWidget* parent) override final;

    /**
     * Create collapsed widget
     * @param parent Parent widget
     * @param widgetFlags Collapsed widget configuration flags
     * @return Pointer to collapsed widget
     */
    virtual QWidget* createCollapsedWidget(QWidget* parent, std::int32_t widgetFlags = 0) const final;

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
    virtual std::int32_t getSortIndex() const final;

    /**
     * Set the sort index
     * @param sortIndex Sorting index
     */
    virtual void setSortIndex(const std::int32_t& sortIndex) final;

    /**
     * Get stretch
     * @return The stretch factor
     */
   virtual std::int32_t getStretch() const final;

    /**
     * Set stretch to \p stretch
     * @param stretch Stretch factor
     */
    virtual void setStretch(const std::int32_t& stretch) final;

public: // Visibility

    /**
     * Get force hidden
     * @return Boolean determining whether the widget action should be forcibly hidden (regardless of the visibility setting in the base QWidgetAction class)
     */
    virtual bool getForceHidden() const final;

    /**
     * Set force hidden to \p forceHidden
     * @param forceHidden Boolean determining whether the widget action should be forcibly hidden (regardless of the enabled visibility in the base QWidgetAction class)
     */
    virtual void setForceHidden(bool forceHidden) final;

    /**
     * Re-implement the isVisible() getter from the base QWidgetAction class to support the force hidden functionality
     * @return Boolean determining whether the widget action is visible or not
     */
    virtual bool isVisible() const final;

public: // Disabled

    /**
     * Get force disabled
     * @return Boolean determining whether the widget action should be forcibly disabled (regardless of the enabled setting in the base QWidgetAction class)
     */
    virtual bool getForceDisabled() const final;

    /**
     * Set force disabled to \p forceDisabled
     * @param forceDisabled Boolean determining whether the widget action should be forcibly disabled (regardless of the enabled setting in the base QWidgetAction class)
     */
    virtual void setForceDisabled(bool forceDisabled) final;

    /**
     * Re-implement the isEnabled() getter from the base QWidgetAction class to support the force disabled functionality
     * @return Boolean determining whether the widget action is enabled or not
     */
    virtual bool isEnabled() const final;

public: // Text

    /**
     * Re-implement the setText(...) setter from the base QWidgetAction class to support updating the location
     * @param text Text to set
     */
    void setText(const QString& text);

public: // Location

    /**
     * Get location
     * @return Path relative to the top-level action
     */
    virtual QString getLocation() const;

private: // Location

    /**
     * Compute the location of the action and update the cached location if it changed
     * @param recursive Whether to also update child actions recursively
     */
    virtual void updateLocation(bool recursive = true) final;

public: // Widget flags

    /** Gets the default widget flags */
    virtual std::int32_t getDefaultWidgetFlags() const final;

    /**
     * Set the widget flags
     * @param widgetFlags Widget flags
     */
    virtual void setDefaultWidgetFlags(const std::int32_t& widgetFlags) final;

    /**
     * Set a single widget flag on/off
     * @param widgetFlag Widget flag to set on/off
     * @param unset Whether to unset the default widget flag
     */
    virtual void setDefaultWidgetFlag(const std::int32_t& widgetFlag, bool unset = false) final;

public: // Highlighting

    /**
     * Get highlighting
     * @return Highlight option
     */
    virtual HighlightOption getHighlighting() const final;

    /**
     * Determine whether the action is in a highlighted state or not
     * @return Boolean determining whether the action is in a highlighted state or not
     */
    virtual bool isHighlighted() const final;

    /**
     * Set highlighting to \p highlighting
     * @param highlighting Highlighting state
     */
    virtual void setHighlighting(const HighlightOption& highlighting) final;

    /**
     * Set highlighted to \p highlighted
     * @param highlighted Boolean determining whether the action is in a normal highlighted state or not
     */
    virtual void setHighlighted(bool highlighted) final;

    /** Convenience method to highlight the action */
    virtual void highlight() final;

    /** Convenience method to un-highlight the action */
    virtual void unHighlight() final;

public: // Scope

    /**
     * Get widget action scope
     * @return Widget action scope enum
     */
    virtual Scope getScope() const final;

    /**
     * Get whether this action is in the private actions pool
     * @return Boolean determining whether this action is in the private actions pool
     */
    virtual bool isPrivate() const final;

    /**
     * Get whether this action is in the public actions pool
     * @return Boolean determining whether this action is in the private public pool
     */
    virtual bool isPublic() const final;

protected: // Connections

    /**
     * Make widget action public (and possibly all of its descendant widget actions)
     * @param recursive Whether to also make all descendant widget actions public
     */
    virtual void makePublic(bool recursive = true) final;

public: // Connections and publishing

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
     * Get whether the action may connect to \p publicAction
     * @param publicAction Pointer to public action
     * @return Boolean determining whether the action may connect to \p publicAction
     */
    virtual bool mayConnectToPublicAction(const WidgetAction* publicAction) const final;

    /**
     * Publish this action so that other actions can connect to it
     * @param name Name of the published widget action (if empty, a configuration dialog will popup)
     * @param allowDuplicateName Boolean determining whether publishing will take place when a public with the same name already exists in the public actions database
     * @return Boolean determining whether the action is successfully published or not
     */
    virtual bool publish(const QString& name = "", bool allowDuplicateName = false) final;

protected: 

    /**
     * Connect this action to a public action
     * @param publicAction Pointer to public action to connect to
     * @param recursive Whether to also connect descendant child actions
     */
    virtual void connectToPublicAction(WidgetAction* publicAction, bool recursive);

    /**
     * Connect this action to a public action
     * @param publicAction Pointer to public action to connect to
     */
    [[deprecated("This function will be deprecated in version 1.0 of ManiVault, Please establish connections with the connections GUI instead.")]]
    virtual void connectToPublicActionByName(const QString& publicActionName) final;

    /**
     * Disconnect this action from its public action
     * @param recursive Whether to also disconnect descendant child actions
     */
    virtual void disconnectFromPublicAction(bool recursive);

public:

    /**
     * Get the public action to which the action is connected
     * @return Pointer to the public action (returns nullptr if not connected to a public action)
     */
    virtual WidgetAction* getPublicAction() final;

    /**
     * Get public copy of the action (other compatible actions can connect to it)
     * @return Pointer to public copy of the action
     */
    virtual WidgetAction* getPublicCopy() const final;

    /**
     * Get connected actions
     * @return Vector of pointers to connected actions
     */
    virtual const WidgetActions getConnectedActions() const final;

    /**
     * Get connected actions
     * @return Vector of pointers to connected actions
     */
    virtual WidgetActions& getConnectedActions() final;

public: // Connection permissions

    /**
     * Get whether a copy of this action may be published and shared, depending on the \p connectionContextFlags
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

    /**
     * Set connection permissions to force none (connections fully disabled, regardless of other connection permission flags)
     * @param recursive Whether to recursively set child connection permissions
     */
    virtual void setConnectionPermissionsToForceNone(bool recursive = false) final;

    /**
     * Set connection permissions to all
     * @param recursive Whether to recursively set child connection permissions
     */
    virtual void setConnectionPermissionsToAll(bool recursive = false) final;

    /**
     * Cache connection permissions
     * @param recursive Whether to recursively cache child connection permissions
     */
    virtual void cacheConnectionPermissions(bool recursive = false) final;

    /**
     * Restore connection permissions
     * @param recursive Whether to recursively restore child connection permissions
     */
    virtual void restoreConnectionPermissions(bool recursive = false) final;

public: // Drag and drop

    /** Start drag process */
    virtual void startDrag() final;

public: // Settings

    /**
     * Determines whether the action can be reset to its default
     * @return Whether the action can be reset to its default
     */
    [[deprecated("This method is a placeholder and not operational yet")]]
    virtual bool isResettable() const final;

    /** Reset to default */
    [[deprecated("This method is a placeholder and not operational yet")]]
    virtual void reset() final;

    /**
     * Set settings prefix
     * @param load Whether to restore settings after setting the prefix
     * @param settingsPrefix Settings prefix
     */
    virtual void setSettingsPrefix(const QString& settingsPrefix, const bool& load = true) final;

    /**
     * Set settings prefix in the context of a plugin (the combined settings prefix will be: Plugins/PluginKind/SettingsPrefix)
     * @param load Whether to restore settings after setting the prefix
     * @param plugin Pointer to plugin context
     * @param settingsPrefix Settings prefix
     */
    virtual void setSettingsPrefix(plugin::Plugin* plugin, const QString& settingsPrefix, const bool& load = true) final;

    /**
     * Get settings prefix
     * @return Settings prefix
     */
    virtual QString getSettingsPrefix() const;

    /** Load from settings (if the settings prefix is set) */
    virtual void loadFromSettings() final;

    /** Save to settings (if the settings prefix is set) */
    virtual void saveToSettings() final;

    /**
     * Find child widget action of which the GUI name contains the search string
     * @param searchString The search string
     * @param recursive Whether to search recursively
     * @return Found vector of pointers to widget action(s)
     */
    virtual QVector<WidgetAction*> findChildren(const QString& searchString, bool recursive = true) const final;
    
public: // Popups

    /**
     * Get size hint of popups (in case of collapsed actions)
     * @return Popup size hint
     */
    virtual QSize getPopupSizeHint() const final;

    /**
     * Set size hint of popups (in case of collapsed actions)
     * @param popupSizeHint Popup size hint
     */
    virtual void setPopupSizeHint(const QSize& popupSizeHint) final;

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

public: // Type string

    /**
     * Get type string
     * @param humanFriendly Whether to return a type string where the namespace is omitted
     * @return Widget action type in string format
     */
    virtual QString getTypeString(bool humanFriendly = false) const final;

protected: // Widgets

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
    virtual void cacheState(const QString& name = "cache") final;

    /**
     * Restore the state of under \p name
     * @param name Name of the cached widget action state to restore
     * @param remove Whether to remove the cache
     */
    virtual void restoreState(const QString& name = "cache", bool remove = true) final;

public: // Studio mode
    
    /**
     * Set studio mode to \p studioMode
     * @param studioMode Boolean determining whether studio mode is on or off
     * @param recursive Boolean determining whether to also apply the studio mode to child actions recursively
     */
    virtual void setStudioMode(bool studioMode, bool recursive = true) final;
    
public: // Font Icon
    
    /**
     * Set the icon using a icon font name (i.e., fontawesome)
     * @param setIcon the name of the icon in fontawesome v5
     */
    virtual void setIconByName(QString namedIcon) final;

private:
    
    /** refresh the icon when a icon font is used */
    void refreshIcon();
    
    /** refresh the icon when a icon font is used */
    void updateCustomStyle();
    
signals:

    /**
     * Signals that the globally unique identifier changed (only happens upon de-serialization of a widget action)
     * @param id Globally unique identifier that changed
     */
    void idChanged(const QString& id);

    /**
     * Signals that the sort index changed to \p sortIndex
     * @param sortIndex Sort index (relative position in group items)
     */
    void sortIndexChanged(std::int32_t sortIndex);

    /**
     * Signals that the stretch changed to \p stretch
     * @param stretch Stretch factor (in action groups)
     */
    void stretchChanged(std::int32_t stretch);

    /**
     * Signals that force hidden changed to \p forceHidden
     * @param forceHidden Boolean determining whether the widget action should be forcibly hidden (regardless of the visibility setting in the base QWidgetAction class)
     */
    void forceHiddenChanged(bool forceHidden);

    /**
     * Signals that force disabled changed to \p forceDisabled
     * @param forceDisabled Boolean determining whether the widget action should be forcibly disabled (regardless of the enabled setting in the base QWidgetAction class)
     */
    void forceDisabledChanged(bool forceDisabled);

    /**
     * Signals that the highlighting options changed to \p highlightOption
     * @param highlightOption Current highlight option
     */
    void highlightingChanged(const HighlightOption& highlightOption);

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
    void actionConnected(WidgetAction* action);

    /**
     * Signals that an action disconnected
     * @param action Action that disconnected
     */
    void actionDisconnected(WidgetAction* action);

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

    /**
     * Signals that the scope changed to \p scope
     * @param scope Scope of the widget action
     */
    void scopeChanged(const Scope& scope);

    /**
     * Signals that the text changed to \p text
     * @param text Action text
     */
    void textChanged(const QString& text);

    /**
     * Signals that the location changed to \p location
     * @param location The path relative to the root in string format
     */
    void locationChanged(const QString& location);

private:
    std::int32_t                _defaultWidgetFlags;            /** Default widget flags which are used to configure newly created widget action widgets */
    std::int32_t                _sortIndex;                     /** Sort index (relative position in group items) */
    std::int32_t                _stretch;                       /** Stretch factor (in group items) */
    bool                        _forceHidden;                   /** When set to true, this action is hidden, regardless of the visibility setting in the base QWidgetAction class */
    bool                        _forceDisabled;                 /** When set to true, this action is read-only, regardless of the enabled setting in the base QWidgetAction class */
    std::int32_t                _connectionPermissions;         /** Allowed connection permissions flags */
    std::int32_t                _cachedConnectionPermissions;   /** Cached connection permissions flags */
    Scope                       _scope;                         /** Determines whether this action is a public (shared) action or not (private) */
    QPointer<WidgetAction>      _publicAction;                  /** Public action to which this action is connected (nullptr if not connected) */
    QVector<WidgetAction*>      _connectedActions;              /** Pointers to widget actions that are connected to this action */
    QString                     _settingsPrefix;                /** If non-empty, the prefix is used to save the contents of the widget action to settings with the Qt settings API */
    HighlightOption             _highlighting;                  /** Highlighting state */
    QSize                       _popupSizeHint;                 /** Size hint of the popup */
    std::int32_t                _configuration;                 /** Configuration flags */
    QMap<QString, QVariant>     _cachedStates;                  /** Maps cache name to state */
    QString                     _location;                      /** The path relative to the root in string format */
    QString                     _namedIcon;                     /** The name of a font awesome icon. When using this the widget can handle icon updates itself, instead of the containing view */

protected:
    friend class mv::AbstractActionsManager;
};

}
}
