// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/WidgetAction.h"
#include "actions/HorizontalGroupAction.h"

#include "util/Video.h"

namespace mv::plugin {
    class Plugin;
}

namespace mv::gui {

/**
 * Note: This action is developed for internal use only
 */
class CORE_EXPORT PluginLearningCenterAction final : public WidgetAction
{
    Q_OBJECT

public:

    /**
     * Construct with pointer to \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE PluginLearningCenterAction(QObject* parent, const QString& title);

    /**
     * Initialize with pointer to \p plugin and \p parent object
     * @param plugin Pointer to plugin
     */
    void initialize(plugin::Plugin* plugin);

public: // Plugin description

    /**
     * Get short description
     * @return Short description in plain text format
     */
    QString getShortDescription() const;

    /**
     * Set short description to \p shortDescription
     * @param shortDescription Short plugin description in plain text format
     */
    void setShortDescription(const QString& shortDescription);

    /**
     * Get long description
     * @return Long description in plain text format
     */
    QString getLongDescription() const;

    /**
     * Set long description to \p longDescription
     * @param longDescription Long plugin description in plain text format
     */
    void setLongDescription(const QString& longDescription);

    /**
     * Get whether the plugin has a description or not
     * @return Boolean determining whether the plugin has a description or not
     */
    bool hasDescription() const;

public: // Help

    /**
     * Get whether the plugin has help information or not
     * @return Boolean determining whether the plugin has help information or not
     */
    bool hasHelp() const;

public: // Videos

    /**
     * Add \p video
     * @param video Video
     */
    void addVideo(const util::Video& video);

    /**
     * Add \p videos
     * @param videos Videos
     */
    void addVideos(const util::Videos& videos);

    /**
     * Add video by \p tag
     * @param tag Video tag (from global learning center video)
     */
    void addVideos(const QString& tag);

    /**
     * Add videos by \p tags
     * @param tags Video tags (from global learning center video)
     */
    void addVideos(const QStringList& tags);

    /**
     * Get videos
     * @return Videos
     */
    const util::Videos& getVideos() const;

public: // Serialization

    /**
     * Load widget action from variant
     * @param variantMap Variant representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant
     * @return Variant representation of the widget action
     */
    QVariantMap toVariantMap() const override;

private:

    /**
     * Get whether PluginLearningCenterAction#_viewPlugin is a view plugin
     * @return Boolean determining whether PluginLearningCenterAction#_plugin is a view plugin or not
     */
    bool isViewPlugin() const;

    /** View plugin description (view depends on the type of plugin) */
    void viewDescription();

    /** View shortcut map (view depends on the type of plugin) */
    void viewShortcutMap();

public: // Action getters

    TriggerAction& getViewDescriptionAction() { return _viewDescriptionAction; }
    TriggerAction& getViewHelpAction() { return _viewHelpAction; }
    TriggerAction& getViewShortcutMapAction() { return _viewShortcutMapAction; }

    const TriggerAction& getViewDescriptionAction() const { return _viewDescriptionAction; }
    const TriggerAction& getViewHelpAction() const { return _viewHelpAction; }
    const TriggerAction& getViewShortcutMapAction() const { return _viewShortcutMapAction; }

signals:

    /**
     * Signals that the short description changed from \p previousShortDescription to \p currentShortDescription
     * @param previousShortDescription Previous short description
     * @param currentShortDescription Current short description
     */
    void shortDescriptionChanged(const QString& previousShortDescription, const QString& currentShortDescription);

    /**
     * Signals that the long description changed from \p previousShortDescription to \p currentShortDescription
     * @param previousLongDescription Previous long description
     * @param currentLongDescription Current long description
     */
    void longDescriptionChanged(const QString& previousLongDescription, const QString& currentLongDescription);

private:
    plugin::Plugin*         _plugin;                        /** Pointer to associated plugin */
    HorizontalGroupAction   _actions;                       /** Learning center actions */
    TriggerAction           _viewDescriptionAction;         /** Trigger action that displays the plugin help */
    TriggerAction           _viewHelpAction;                /** Trigger action that displays the plugin description */
    TriggerAction           _viewShortcutMapAction;         /** Trigger action that displays the plugin shortcut map */
    QString                 _shortDescription;              /** Short plugin description in plain text format */
    QString                 _longDescription;               /** Long plugin description in HTML-formatted text */
    util::Videos            _videos;                        /** Plugin related videos */
    QPointer<QWidget>       _descriptionOverlayWidget;      /** Guarded pointer to description overlay widget  */
    QPointer<QWidget>       _shortcutMapOverlayWidget;      /** Guarded pointer to shortcut cheatsheet overlay widget  */
};

}

Q_DECLARE_METATYPE(mv::gui::PluginLearningCenterAction)

inline const auto pluginLearningCenterActionMetaTypeId = qRegisterMetaType<mv::gui::PluginLearningCenterAction*>("mv::gui::PluginLearningCenterAction");