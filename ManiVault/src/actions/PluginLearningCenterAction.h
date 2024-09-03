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
class CORE_EXPORT PluginLearningCenterAction : public WidgetAction
{
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

public: // Help

    /**
     * Get whether the plugin has help information or not
     * @return Boolean determining whether the plugin has help information or not
     */
    bool hasHelp() const;

public: // Serialization

    /**
     * Load widget action from variant
     * @param Variant representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant
     * @return Variant representation of the widget action
     */
    QVariantMap toVariantMap() const override;

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

public: // Action getters

    TriggerAction& getViewDescriptionAction() { return _viewDescriptionAction; }
    TriggerAction& getViewHelpAction() { return _viewHelpAction; }
    TriggerAction& getViewShortcutMapAction() { return _viewShortcutMapAction; }

    const TriggerAction& getViewDescriptionAction() const { return _viewDescriptionAction; }
    const TriggerAction& getViewHelpAction() const { return _viewHelpAction; }
    const TriggerAction& getViewShortcutMapAction() const { return _viewShortcutMapAction; }


private:
    plugin::Plugin*         _plugin;                    /** Pointer to associated plugin */
    HorizontalGroupAction   _actions;                   /** Learning center actions */
    TriggerAction           _viewDescriptionAction;     /** Trigger action that displays the plugin help */
    TriggerAction           _viewHelpAction;            /** Trigger action that displays the plugin description */
    TriggerAction           _viewShortcutMapAction;     /** Trigger action that displays the plugin shortcut map */
    util::Videos            _videos;                    /** Plugin related videos */
//
//    friend class PluginFactory;
};

}

Q_DECLARE_METATYPE(mv::gui::PluginLearningCenterAction)

inline const auto pluginLearningCenterActionMetaTypeId = qRegisterMetaType<mv::gui::PluginLearningCenterAction*>("mv::gui::PluginLearningCenterAction");