// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/WidgetAction.h"
#include "actions/HorizontalGroupAction.h"
#include "actions/ToggleAction.h"
#include "actions/TriggerAction.h"
#include "actions/OptionAction.h"

#include "util/LearningCenterVideo.h"
#include "util/LearningCenterTutorial.h"

namespace mv::plugin {
    class Plugin;
    class ViewPlugin;
}

namespace mv::gui {

class ViewPluginLearningCenterOverlayWidget;

/**
 * Plugin learning center action
 *
 * All plugin-related action interface
 *
 * Note: This action is developed for internal use only
 *
 * @author Thomas Kroes
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

    /**
     * Get the context menu for the action
     * @param parent Parent widget
     * @return Context menu
     */
    QMenu* getContextMenu(QWidget* parent = nullptr) override;

    /**
     * Get the context menu for the alignment
     * @param parent Parent widget
     * @return Pointer to alignment context menu
     */
    QMenu* getAlignmentContextMenu(QWidget* parent = nullptr);

    /**
     * Get view plugin overlay widget
     * @return Pointer to plugin overlay widget (maybe nullptr)
     */
    ViewPluginLearningCenterOverlayWidget* getViewPluginOverlayWidget() const;

    /**
     * Get view plugin overlay alignment as a Qt flag
     * @return Alignment flag
     */
    Qt::Alignment getAlignment() const;

    /**
     * Set view plugin overlay alignment to \p alignment
     * @param alignment Alignment flag
     */
    void setAlignment(const Qt::Alignment& alignment);

protected:

    /** Create the view plugin overlay widget (should only be called by the view plugin) */
    void createViewPluginOverlayWidget();

public: // Plugin description

    /**
     * Get plugin title
     * @return Human-readable plugin title in plain text format
     */
    QString getPluginTitle() const;

    /**
     * Set plugin title to \p pluginTitle
     * @param pluginTitle Human-readable plugin title in plain text format
     */
    void setPluginTitle(const QString& pluginTitle);

    /**
     * Get short description
     * @return Short description in plain text format
     */
    QString getShortDescription() const;

    /**
     * Set short description to \p shortDescription
     * @param shortDescription Short plugin description in plain text format
     */
    void setShortDescription(const QString& shortDescription) const;

    /**
     * Get long description
     * @return Long description in plain text format
     */
    QString getLongDescription() const;

    /**
     * Set long description to \p longDescription
     * @param longDescription Long plugin description in plain text format
     */
    void setLongDescription(const QString& longDescription) const;

    /**
     * Get Markdown-formatted long description
     * @return Long description in Markdown format
     */
    QString getLongDescriptionMarkdown() const;

    /**
     * Set Markdown-formatted long description to \p longDescriptionMarkdown
     * @param longDescriptionMarkdown Long plugin description in Markdown format
     */
    void setLongDescriptionMarkdown(const QString& longDescriptionMarkdown) const;

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
     * @param video Pointer to video
     */
    void addVideo(const util::LearningCenterVideo* video);

    /**
     * Add \p videos
     * @param videos Videos
     */
    void addVideos(const util::LearningCenterVideos& videos);

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
    const util::LearningCenterVideos& getVideos() const;

public: // Tutorials

    /**
     * Add \p tutorial
     * @param tutorial Pointer to tutorial
     */
    void addTutorial(const util::LearningCenterTutorial* tutorial);

    /**
     * Add \p tutorials
     * @param tutorials Tutorials
     */
    void addTutorials(const util::LearningCenterTutorials& tutorials);

    /**
     * Add tutorial by \p tag
     * @param tag tutorial tag (from global learning center tutorial)
     */
    void addTutorials(const QString& tag);

    /**
     * Add tutorials by \p tags
     * @param tags Tutorial tags (from global learning center tutorial)
     */
    void addTutorials(const QStringList& tags);

    /**
     * Get tutorials
     * @return tutorials
     */
    const util::LearningCenterTutorials& getTutorials() const;

private:

    /**
     * Get whether PluginLearningCenterAction#_viewPlugin is a view plugin
     * @return Boolean determining whether PluginLearningCenterAction#_plugin is a view plugin or not
     */
    bool isViewPlugin() const;

    /**
     * Get view plugin
     * @return Pointer to view plugin (maybe nullptr)
     */
    mv::plugin::ViewPlugin* getViewPlugin() const;

    /** View shortcuts (view depends on the type of plugin) */
    void viewShortcuts() const;

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

public: // Action getters

    TriggerAction& getViewDescriptionAction() { return _viewDescriptionAction; }
    TriggerAction& getViewHelpAction() { return _viewHelpAction; }
    TriggerAction& getViewShortcutsAction() { return _viewShortcutsAction; }
    ToggleAction& getToolbarVisibleAction() { return _toolbarVisibleAction; }
    TriggerAction& getHideToolbarAction() { return _hideToolbarAction; }
    OptionAction& getAlignmentAction() { return _alignmentAction; }
    TriggerAction& getMoveToTopLeftAction() { return _moveToTopLeftAction; }
    TriggerAction& getMoveToTopRightAction() { return _moveToTopRightAction; }
    TriggerAction& getMoveToBottomLeftAction() { return _moveToBottomLeftAction; }
    TriggerAction& getMoveToBottomRightAction() { return _moveToBottomRightAction; }

    const TriggerAction& getViewDescriptionAction() const { return _viewDescriptionAction; }
    const TriggerAction& getViewHelpAction() const { return _viewHelpAction; }
    const TriggerAction& getViewShortcutsAction() const { return _viewShortcutsAction; }
    const ToggleAction& getToolbarVisibleAction() const { return _toolbarVisibleAction; }
    const TriggerAction& getHideToolbarAction() const { return _hideToolbarAction; }
    const OptionAction& getAlignmentAction() const { return _alignmentAction; }
    const TriggerAction& getMoveToTopRightAction() const { return _moveToTopRightAction; }
    const TriggerAction& getMoveToBottomLeftAction() const { return _moveToBottomLeftAction; }
    const TriggerAction& getMoveToBottomRightAction() const { return _moveToBottomRightAction; }

signals:

    /**
     * Signals that the plugin title changed to \p pluginTitle
     * @param pluginTitle Plugin title
     */
    void pluginTitleChanged(const QString& pluginTitle);

    /**
     * Signals that the short description changed from \p previousShortDescription to \p currentShortDescription
     * @param previousShortDescription Previous short description
     * @param currentShortDescription Current short description
     */
    void shortDescriptionChanged(const QString& previousShortDescription, const QString& currentShortDescription);

    /**
     * Signals that the HTML-formatted long description changed from \p previousLongDescription to \p currentLongDescription
     * @param previousLongDescription Previous long description in HTML format
     * @param currentLongDescription Current long description in HTML format
     */
    void longDescriptionChanged(const QString& previousLongDescription, const QString& currentLongDescription);

    /**
     * Signals that the Markdown-formatted long description format changed from \p previousLongDescriptionMarkdown to \p currentLongDescriptionMarkdown
     * @param previousLongDescriptionMarkdown Previous long description in Markdown format
     * @param currentLongDescriptionMarkdown Current long description in Markdown format
     */
    void longDescriptionMarkdownChanged(const QString& previousLongDescriptionMarkdown, const QString& currentLongDescriptionMarkdown);

public:
    static const QStringList alignmentOptions;                                      /** Names of the supported alignments in the case of a view plugin */
    static const std::vector<Qt::Alignment> alignmentFlags;                         /** Supported Qt alignment flags in the case of a view plugin */

private:
    plugin::Plugin*                             _plugin;                            /** Pointer to associated plugin */
    HorizontalGroupAction                       _actions;                           /** Learning center actions */
    TriggerAction                               _viewDescriptionAction;             /** Trigger action that displays the plugin help */
    TriggerAction                               _viewHelpAction;                    /** Trigger action that displays the plugin description */
    TriggerAction                               _viewShortcutsAction;               /** Trigger action that displays the plugin shortcut map */
    ToggleAction                                _toolbarVisibleAction;              /** Toggles toolbar widget visibility */
    TriggerAction                               _hideToolbarAction;                 /** Hides the view plugin overlay toolbar widget */
    OptionAction                                _alignmentAction;                   /** Determines the view plugin overlay alignment */
    TriggerAction                               _moveToTopLeftAction;               /** Trigger action that moves the view plugin overlay to the top-left of the widget */
    TriggerAction                               _moveToTopRightAction;              /** Trigger action that moves the view plugin overlay to the top-right of the widget */
    TriggerAction                               _moveToBottomLeftAction;            /** Trigger action that moves the view plugin overlay to the bottom-left of the widget */
    TriggerAction                               _moveToBottomRightAction;           /** Trigger action that moves the view plugin overlay to the bottom-right of the widget */
    QString                                     _pluginTitle;                       /** Human-readable plugin title in plain text format */
    util::LearningCenterVideos                  _videos;                            /** Plugin related videos */
    util::LearningCenterTutorials               _tutorials;                         /** Plugin related tutorials */
    ViewPluginLearningCenterOverlayWidget*      _learningCenterOverlayWidget;       /** Add learning center overlay widget */

    friend class plugin::ViewPlugin;
};

}

Q_DECLARE_METATYPE(mv::gui::PluginLearningCenterAction)

inline const auto pluginLearningCenterActionMetaTypeId = qRegisterMetaType<mv::gui::PluginLearningCenterAction*>("mv::gui::PluginLearningCenterAction");
