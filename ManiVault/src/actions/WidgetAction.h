// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetActionWidget.h"
#include "WidgetActionBadge.h"
#include "WidgetActionDrag.h"

#include "util/Serializable.h"
#include "util/styledIcon.h"

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

template<typename WidgetActionType>
using WidgetActionsOfType = QList<WidgetActionType*>;

using WidgetActions = WidgetActionsOfType<WidgetAction>;

using ConstWidgetActions = QList<const WidgetAction*>;
using WidgetConfigurationFunction = std::function<void(WidgetAction*, QWidget*)>;

/**
 * Widget action class
 *
 * Base class for custom widget actions
 * 
 * @author Thomas Kroes
 */
class CORE_EXPORT WidgetAction : public QWidgetAction, public util::Serializable
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
        HiddenInActionContextMenu   = 0x00001,      /** Do not show this action in the context menu of another action */
        NoLabelInGroup              = 0x00002,      /** Action will not have a label when it is displayed in a group */
        ForceCollapsedInGroup       = 0x00008,      /** Action will be collapsed in a horizontal group (or toolbar), no matter the circumstances */
        ForceExpandedInGroup        = 0x00010,      /** Action will be expanded in a horizontal group (or toolbar), no matter the circumstances */
        ToolButtonAutoRaise         = 0x00020,      /** Sets auto-raise to true when a widget action is collapsed and a tool button is used for the popup functionality */
        NoGroupBoxInPopupLayout     = 0x00040,      /** Prevent group box in popup layout */

        User                        = 0x00100,      /** And beyond for custom configuration flags */

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
    ~WidgetAction() override;

public: // Hierarchy queries

    /**
     * Get parent action of \p WidgetActionType
     * @return Pointer to parent action (nullptr if not found or if dynamic cast fails)
     */
    template<typename WidgetActionType = WidgetAction>
    WidgetActionType* getParent() const {
        return dynamic_cast<WidgetActionType*>(parent());
    }

    /**
     * Establish whether the action has a parent
     * @return Boolean determining whether the action has a parent or not
     */
    bool hasParent() const {
        return getParent();
    }

    /**
     * Establish whether the action has a parent of \p WidgetActionType
     * @return Boolean determining whether the action has a parent of \p WidgetActionType or not
     */
    template<typename WidgetActionType = WidgetAction>
    bool hasParent() const {
        return getParent<WidgetActionType>();
    }

    /**
     * Get ancestors of \p WidgetActionType
     * @return Pointers to ancestors of \p WidgetActionType (bottom-up)
     */
    template<typename WidgetActionType = WidgetAction>
    WidgetActionsOfType<WidgetActionType> getAncestors() const {
        WidgetActionsOfType<WidgetActionType> ancestors;

        auto currentParent = dynamic_cast<WidgetActionType*>(parent());

        while (currentParent) {
            ancestors << currentParent;

            currentParent = dynamic_cast<WidgetActionType*>(currentParent->parent());
        }

        return ancestors;
    }

    /**
     * Get child actions of \p WidgetActionType, possibly \p recursively
     * @param recursively Get children recursively
     * @return Vector of pointers to child actions of \p WidgetActionType
     */
    template<typename WidgetActionType = WidgetAction>
    WidgetActionsOfType<WidgetActionType> getChildren(bool recursively = false) const {
        WidgetActionsOfType<WidgetActionType> childrenOfType;

        for (auto actionChild : findChildren<QObject*>(recursively ? Qt::FindChildrenRecursively : Qt::FindDirectChildrenOnly))
            if (auto childOfType = dynamic_cast<WidgetActionType*>(actionChild))
                childrenOfType << childOfType;

        return childrenOfType;
    }

    /**
     * Get child actions of \p WidgetActionType, up until \p maxDepth
     * @param maxDepth Max depth of included children (fully recursive when -1)
     * @return Vector of pointers to child actions of \p WidgetActionType
     */
    template<typename WidgetActionType = WidgetAction>
    WidgetActionsOfType<WidgetActionType> getChildren(std::int32_t maxDepth) const {
        WidgetActionsOfType<WidgetActionType> childrenOfType;

        for (auto childOfType : getChildren<WidgetActionType>(true))
            if (maxDepth == -1 || childOfType->getDepth() <= maxDepth)
                childrenOfType << childOfType;

        return childrenOfType;
    }

    /**
     * Get number of children of \p WidgetActionType, possibly \p recursively
     * @param recursively Count recursively
     * @return Number of children
     */
    template<typename WidgetActionType = WidgetAction>
    std::uint32_t getNumberOfChildren(bool recursively = false) const {
        return static_cast<std::uint32_t>(getChildren<WidgetActionType>(recursively).count());
    }

    /**
     * Establishes whether this action has any children of \p WidgetActionType
     * @return Boolean determining whether the item has any children
     */
    template<typename WidgetActionType = WidgetAction>
    bool hasChildren() const {
        return getNumberOfChildren<WidgetActionType>() > 0;
    }

    /**
     * Get the depth of the action w.r.t. its furthest ancestor of \p WidgetActionType
     * @return Item depth (root starts at zero)
     */
    template<typename WidgetActionType = WidgetAction>
    std::int32_t getDepth() const {
        return static_cast<std::int32_t>(getAncestors<WidgetActionType>().count());
    }

    /**
     * Find child of \p WidgetActionType by \p path
     * @return Pointer to child action of \p WidgetActionType, otherwise nullptr
     */
    template<typename WidgetActionType = WidgetAction>
    WidgetActionType* findChildByPath(const QString& path) const {
        QMap<QString, WidgetAction*> childrenByPath;

        for (auto child : getChildren(true))
            childrenByPath[child->getLocation(true)] = child;

        const auto prefixedPath = QString("%1/%2").arg(text(), path);

        if (childrenByPath.contains(prefixedPath))
            return dynamic_cast<WidgetActionType*>( childrenByPath[prefixedPath]);

        return nullptr;
    }

    /**
     * Determine whether this action is a child of \p action of \p WidgetActionType
     * @param action Action to check for
     * @return Boolean determining whether \p action is an ancestor or not
     */
    template<typename WidgetActionType = WidgetAction>
    bool isChildOf(WidgetAction* action) const {
        return getAncestors<WidgetActionType>().contains(action);
    }

    /**
     * Determine whether this action is a child of \p actions of \p WidgetActionType
     * @param actions Actions to check for
     * @return Boolean determining whether this action is child of one of \p actions or not
     */
    template<typename WidgetActionType = WidgetAction>
    bool isChildOf(WidgetActions actions) const {
        for (auto action : actions)
            if (action != this && isChildOf<WidgetActionType>(action))
                return true;

        return false;
    }

public: // Location

    /**
     * Get location and possibly \p recompute it
     * @param recompute Whether to re-compute the location
     * @return Path relative to the top-level action
     */
    virtual QString getLocation(bool recompute = false) const;

private: // Location

    /**
     * Compute the location of the action and update the cached location if it changed
     * @param recursive Whether to also update child actions recursively
     */
    void updateLocation(bool recursive = true);

public:

    /**
     * Print the paths of children of \p WidgetActionType
     * @param maxDepth Maximum depth to print
     */
    template<typename WidgetActionType = WidgetAction>
    void printChildren(std::int32_t maxDepth = -1) const {
        for (auto child : getChildren<WidgetActionType>(maxDepth)) {
            auto segments = child->getLocation(true).split("/");

            if (segments.isEmpty())
                continue;

            segments.removeFirst();

            if (!segments.isEmpty())
                qDebug() << segments.join("/");
        }
    }

public:

    /**
     * Establish whether this action is positioned at the top of the hierarchy
     * @return Boolean determining whether this action is positioned at the top of the hierarchy
     */
    bool isRoot() const;

    /**
     * Establish whether this action is positioned at the bottom of the hierarchy
     * @return Boolean determining whether this action is positioned at the bottom of the hierarchy
     */
    bool isLeaf() const;

public: // Widgets

    /**
     * Create widget with pointer to \p parent widget
     * @param parent Parent widget
     * @return Pointer to created widget
     */
    QWidget* createWidget(QWidget* parent) override final;

    /**
     * Create widget with pointer to \p parent widget and \p widgetFlags
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags
     * @return Pointer to created widget
     */
    QWidget* createWidget(QWidget* parent, const std::int32_t& widgetFlags);

    /**
     * Create widget with pointer to \p parent widget, \p widgetFlags and \p widgetConfigurationFunction
     * @param parent Parent widget
     * @param widgetFlags Widget flags
     * @param widgetConfigurationFunction Configuration function to run after the widget is created (overrides WidgetAction#_widgetConfigurationFunction)
     * @return Pointer to created widget
     */
    QWidget* createWidget(QWidget* parent, const std::int32_t& widgetFlags, const WidgetConfigurationFunction& widgetConfigurationFunction);

    /**
     * Create widget with pointer to \p parent widget and \p widgetConfigurationFunction
     * @param parent Parent widget
     * @param widgetConfigurationFunction Configuration function to run after the widget is created (overrides WidgetAction#_widgetConfigurationFunction)
     * @return Pointer to created widget
     */
    QWidget* createWidget(QWidget* parent, const WidgetConfigurationFunction& widgetConfigurationFunction);

    /**
     * Create collapsed widget
     * @param parent Parent widget
     * @return Pointer to collapsed widget
     */
    QWidget* createCollapsedWidget(QWidget* parent) const;

    /**
     * Create collapsed widget
     * @param parent Parent widget
     * @param widgetConfigurationFunction Configuration function to run after the widget is created (overrides WidgetAction#_widgetConfigurationFunction)
     * @return Pointer to collapsed widget
     */
    QWidget* createCollapsedWidget(QWidget* parent, const WidgetConfigurationFunction& widgetConfigurationFunction) const;

    /**
     * Create label widget
     * @param parent Parent widget
     * @param widgetFlags Label widget configuration flags
     * @return Pointer to widget
     */
    QWidget* createLabelWidget(QWidget* parent, const std::int32_t& widgetFlags = 0x00001) const;

    /**
     * Get the context menu for the action
     * @param parent Parent widget
     * @return Context menu
     */
    virtual QMenu* getContextMenu(QWidget* parent = nullptr);

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

    /**
     * Get widget configuration function
     * @return Function that is called right after a widget action widget is created (useful for manual manipulation of the generated widget)
     */
    WidgetConfigurationFunction getWidgetConfigurationFunction();

    /**
     * Set widget configuration function to \p widgetConfigurationFunction
     * @param widgetConfigurationFunction This function is called right after a widget action widget is created (useful for manual manipulation of the generated widget)
     */
    void setWidgetConfigurationFunction(const WidgetConfigurationFunction& widgetConfigurationFunction);

public: // Visibility

    /**
     * Get force hidden
     * @return Boolean determining whether the widget action should be forcibly hidden (regardless of the visibility setting in the base QWidgetAction class)
     */
    bool getForceHidden() const;

    /**
     * Set force hidden to \p forceHidden
     * @param forceHidden Boolean determining whether the widget action should be forcibly hidden (regardless of the enabled visibility in the base QWidgetAction class)
     */
    void setForceHidden(bool forceHidden);

    /**
     * Re-implement the isVisible() getter from the base QWidgetAction class to support the force hidden functionality
     * @return Boolean determining whether the widget action is visible or not
     */
    bool isVisible() const;

public: // Disabled

    /**
     * Get force disabled
     * @return Boolean determining whether the widget action should be forcibly disabled (regardless of the enabled setting in the base QWidgetAction class)
     */
    bool getForceDisabled() const;

    /**
     * Set force disabled to \p forceDisabled
     * @param forceDisabled Boolean determining whether the widget action should be forcibly disabled (regardless of the enabled setting in the base QWidgetAction class)
     */
    void setForceDisabled(bool forceDisabled);

    /**
     * Re-implement the isEnabled() getter from the base QWidgetAction class to support the force disabled functionality
     * @return Boolean determining whether the widget action is enabled or not
     */
    bool isEnabled() const;

public: // Text

    /**
     * Re-implement the setText(...) setter from the base QWidgetAction class to support updating the location
     * @param text Text to set
     */
    void setText(const QString& text);

public: // Widget flags

    /** Gets the default widget flags */
    std::int32_t getDefaultWidgetFlags() const;

    /**
     * Set the widget flags
     * @param defaultWidgetFlags Widget flags
     */
    void setDefaultWidgetFlags(std::int32_t defaultWidgetFlags);

    /**
     * Set a single widget flag on/off
     * @param defaultWidgetFlag Widget flag to set on/off
     * @param unset Whether to unset the default widget flag
     */
    void setDefaultWidgetFlag(std::int32_t defaultWidgetFlag, bool unset = false);

public: // Highlighting

    /**
     * Get highlighting
     * @return Highlight option
     */
    HighlightOption getHighlighting() const;

    /**
     * Determine whether the action is in a highlighted state or not
     * @return Boolean determining whether the action is in a highlighted state or not
     */
    bool isHighlighted() const;

    /**
     * Set highlighting to \p highlighting
     * @param highlighting Highlighting state
     */
    void setHighlighting(const HighlightOption& highlighting);

    /**
     * Set highlighted to \p highlighted
     * @param highlighted Boolean determining whether the action is in a normal highlighted state or not
     */
    void setHighlighted(bool highlighted);

    /** Convenience method to highlight the action */
    void highlight();

    /** Convenience method to un-highlight the action */
    void unHighlight();

public: // Scope

    /**
     * Get widget action scope
     * @return Widget action scope enum
     */
    Scope getScope() const;

    /**
     * Get whether this action is in the private actions pool
     * @return Boolean determining whether this action is in the private actions pool
     */
    bool isPrivate() const;

    /**
     * Get whether this action is in the public actions pool
     * @return Boolean determining whether this action is in the private public pool
     */
    bool isPublic() const;

protected: // Connections

    /**
     * Make widget action public (and possibly all of its descendant widget actions)
     * @param recursive Whether to also make all descendant widget actions public
     */
    void makePublic(bool recursive = true);

public: // Connections and publishing

    /**
     * Get whether the action is published
     * @return Boolean indicating whether the action is published
     */
    bool isPublished() const;

    /**
     * Get whether the action is connect to a public action
     * @return Boolean indicating whether the action is connect to a public action
     */
    bool isConnected() const;

    /**
     * Get whether the action may connect to \p publicAction
     * @param publicAction Pointer to public action
     * @return Boolean determining whether the action may connect to \p publicAction
     */
    bool mayConnectToPublicAction(const WidgetAction* publicAction) const;

    /**
     * Publish this action so that other actions can connect to it
     * @param name Name of the published widget action (if empty, a configuration dialog will popup)
     * @param allowDuplicateName Boolean determining whether publishing will take place when a public with the same name already exists in the public actions database
     * @return Boolean determining whether the action is successfully published or not
     */
    bool publish(const QString& name = "", bool allowDuplicateName = false);

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
    void connectToPublicActionByName(const QString& publicActionName);

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
    WidgetAction* getPublicAction();

    /**
     * Get public copy of the action (other compatible actions can connect to it)
     * @return Pointer to public copy of the action
     */
    WidgetAction* getPublicCopy() const;

    /**
     * Get connected actions
     * @return Vector of pointers to connected actions
     */
    const WidgetActions getConnectedActions() const;

    /**
     * Get connected actions
     * @return Vector of pointers to connected actions
     */
    WidgetActions& getConnectedActions();

public: // Connection permissions

    /**
     * Get whether a copy of this action may be published and shared, depending on the \p connectionContextFlags
     * @param connectionContextFlags The context from which the action will be published (API and/or GUI)
     * @return Boolean determining whether a copy of this action may published and shared, depending on the \p connectionContextFlags
     */
    bool mayPublish(ConnectionContextFlag connectionContextFlags) const;

    /**
     * Get whether this action may connect to a public action, depending on the \p connectionContextFlags
     * @param connectionContextFlags The context from which the connection will be made (API and/or GUI)
     * @return Boolean determining whether this action may connect to a public action, depending on the \p connectionContextFlags
     */
    bool mayConnect(ConnectionContextFlag connectionContextFlags) const;

    /**
     * Get whether this action may disconnect from a public action, depending on the \p connectionContextFlags
     * @param connectionContextFlags The context from which the disconnection will be initiated (API and/or GUI)
     * @return Boolean determining whether this action may disconnect from a public action, depending on the \p connectionContextFlags
     */
    bool mayDisconnect(ConnectionContextFlag connectionContextFlags) const;

    /**
     * Get connection permission flags
     * @return Connection permission flags
     */
    std::int32_t getConnectionPermissions() const;

    /**
     * Check whether \p connectionPermissionsFlag is set or not
     * @param connectionPermissionsFlag Connection permissions flag
     * @return Boolean determining whether \p connectionPermissionsFlag is set or not
     */
    bool isConnectionPermissionFlagSet(ConnectionPermissionFlag connectionPermissionsFlag);

    /**
     * Set connection permissions flag
     * @param connectionPermissionsFlag Connection permissions flag to set
     * @param unset Whether to unset the connection permissions flag
     * @param recursive Whether to recursively set child connection permissions
     */
    void setConnectionPermissionsFlag(ConnectionPermissionFlag connectionPermissionsFlag, bool unset = false, bool recursive = false);

    /**
     * Set connection permissions
     * @param connectionPermissions Connection permissions value
     * @param recursive Whether to recursively set child connection permissions
     */
    void setConnectionPermissions(std::int32_t connectionPermissions, bool recursive = false);

    /**
     * Reset connection permissions to none
     * @param recursive Whether to recursively set child connection permissions
     */
    void setConnectionPermissionsToNone(bool recursive = false);

    /**
     * Set connection permissions to force none (connections fully disabled, regardless of other connection permission flags)
     * @param recursive Whether to recursively set child connection permissions
     */
    void setConnectionPermissionsToForceNone(bool recursive = false);

    /**
     * Set connection permissions to all
     * @param recursive Whether to recursively set child connection permissions
     */
    void setConnectionPermissionsToAll(bool recursive = false);

    /**
     * Cache connection permissions
     * @param recursive Whether to recursively cache child connection permissions
     */
    void cacheConnectionPermissions(bool recursive = false);

    /**
     * Restore connection permissions
     * @param recursive Whether to recursively restore child connection permissions
     */
    void restoreConnectionPermissions(bool recursive = false);

public: // Settings

    /**
     * Determines whether the action can be reset to its default
     * @return Whether the action can be reset to its default
     */
    [[deprecated("This method is a placeholder and not operational yet")]]
    bool isResettable() const;

    /** Reset to default */
    [[deprecated("This method is a placeholder and not operational yet")]]
    void reset();

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
    virtual QString getSettingsPrefix() const;

    /** Load from settings (if the settings prefix is set) */
    void loadFromSettings();

    /** Save to settings (if the settings prefix is set) */
    void saveToSettings();

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

    /**
     * Get override size hint
     * @return Override size hint
     */
    [[deprecated("This method is a placeholder and not operational yet")]]
    QSize getOverrideSizeHint() const;

    /**
     * Set override size hint
     * @param overrideSizeHint Override size hint
     */
    [[deprecated("This method is a placeholder and not operational yet")]]
    void setOverrideSizeHint(const QSize& overrideSizeHint);

public: // Configuration flags

    /**
     * Get configuration
     * @return Configuration
     */
    std::int32_t getConfiguration() const;

    /**
     * Check whether \p configurationFlag is set or not
     * @param configurationFlag Configuration flag
     * @return Boolean determining whether \p configurationFlag is set or not
     */
    bool isConfigurationFlagSet(ConfigurationFlag configurationFlag) const;

    /**
     * Set configuration flag
     * @param configurationFlag Configuration flag to set
     * @param unset Whether to unset the \p configurationFlag flag
     * @param recursive Whether to recursively set child child configuration flag
     */
    void setConfigurationFlag(ConfigurationFlag configurationFlag, bool unset = false, bool recursive = false);

    /**
     * Set configuration
     * @param configuration Configuration value
     * @param recursive Whether to recursively set child child configuration flag
     */
    void setConfiguration(std::int32_t configuration, bool recursive = false);

public: // Type string

    /**
     * Get type string
     * @param humanFriendly Whether to return a type string where the namespace is omitted
     * @return Widget action type in string format
     */
    QString getTypeString(bool humanFriendly = false) const;

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
     * Determine whether cache with name exists
     * @param name Name to use for the cached widget action state
     */
    bool hasCacheState(const QString& name) const;

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

public: // Studio mode
    
    /**
     * Set studio mode to \p studioMode
     * @param studioMode Boolean determining whether studio mode is on or off
     * @param recursive Boolean determining whether to also apply the studio mode to child actions recursively
     */
    void setStudioMode(bool studioMode, bool recursive = true);
    
public: // Styled icon

    /**
     * Set the icon by \p iconName and use the default icon font and version
     * @param iconName Name of the icon
     */
    void setIconByName(const QString& iconName);

    /**
     * Set the icon by \p iconName and possibly override the default \p iconFontName and \p iconFontVersion
     * @param iconName Name of the icon
     * @param iconFontName Name of the icon font
     * @param iconFontVersion Version of the icon font
     */
    void setIconByName(const QString& iconName, const QString& iconFontName, const util::Version& iconFontVersion);

    /**
     * Get the icon
     * @return Reference to named icon
     */
    util::StyledIcon& getIcon();

public: // Badge-related

    /**
     * Get badge
     * @return Reference to the widget action badge
     */
    WidgetActionBadge& getBadge();

public: // Drag-related

    /**
     * Get drag
     * @return Reference to the widget action drag
     */
    WidgetActionDrag& getDrag();

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

    /**
     * Signals that the widget configuration function changed to \p widgetConfigurationFunction
     * @param widgetConfigurationFunction New widget configuration function
     */
    void widgetConfigurationFunctionChanged(WidgetConfigurationFunction& widgetConfigurationFunction);

private:
    std::int32_t                    _defaultWidgetFlags;            /** Default widget flags which are used to configure newly created widget action widgets */
    std::int32_t                    _sortIndex;                     /** Sort index (relative position in group items) */
    std::int32_t                    _stretch;                       /** Stretch factor (in group items) */
    bool                            _forceHidden;                   /** When set to true, this action is hidden, regardless of the visibility setting in the base QWidgetAction class */
    bool                            _forceDisabled;                 /** When set to true, this action is read-only, regardless of the enabled setting in the base QWidgetAction class */
    std::int32_t                    _connectionPermissions;         /** Allowed connection permissions flags */
    std::int32_t                    _cachedConnectionPermissions;   /** Cached connection permissions flags */
    Scope                           _scope;                         /** Determines whether this action is a public (shared) action or not (private) */
    QPointer<WidgetAction>          _publicAction;                  /** Public action to which this action is connected (nullptr if not connected) */
    QVector<WidgetAction*>          _connectedActions;              /** Pointers to widget actions that are connected to this action */
    QString                         _settingsPrefix;                /** If non-empty, the prefix is used to save the contents of the widget action to settings with the Qt settings API */
    HighlightOption                 _highlighting;                  /** Highlighting state */
    QSize                           _popupSizeHint;                 /** Size hint of the popup */
    QSize                           _overrideSizeHint;              /** Override size hint (use with caution) */
    std::int32_t                    _configuration;                 /** Configuration flags */
    QMap<QString, QVariant>         _cachedStates;                  /** Maps cache name to state */
    QString                         _location;                      /** The path relative to the root in string format */
    util::StyledIcon                _styledIcon;                    /** Theme aware icon */
    WidgetConfigurationFunction     _widgetConfigurationFunction;   /** When set, this function is called right after any widget action widget is created (useful for manual manipulation of the generated widget) */
    WidgetActionBadge               _badge;                         /** Badge configuration */
    WidgetActionDrag                _drag;                          /** Drag behaviour */

protected:
    friend class mv::AbstractActionsManager;
};

}
}
