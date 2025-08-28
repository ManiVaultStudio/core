// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "util/DockArea.h"

#include "actions/TriggerAction.h"
#include "actions/ToggleAction.h"
#include "actions/OptionsAction.h"
#include "actions/LockingAction.h"
#include "actions/PresetsAction.h"
#include "actions/ViewPluginSamplerAction.h"
#include "actions/ViewPluginHeadsUpDisplayAction.h"

#include "Plugin.h"
#include "Task.h"

namespace mv::gui {
    class ViewPluginTriggerAction;
    class ToolbarAction;
}

namespace mv::plugin
{

class CORE_EXPORT ViewPlugin : public Plugin
{
    Q_OBJECT

public:

    using ActionWidgetPair = std::pair<QPointer<WidgetAction>, QPointer<QWidget>>;

    /**
     * Constructor
     * @param factory Pointer to plugin factory
     */
    ViewPlugin(const PluginFactory* factory);

    /** Perform startup initialization */
    void init() override;

    /**
     * Load one (or more) datasets in the view
     * @param datasets Dataset(s) to load
     */
    virtual void loadData(const Datasets& datasets);

    /**
     * Get widget representation of the plugin
     * @return Widget representation of the plugin
     */
    QWidget& getWidget();

    /**
     * Get whether this plugin is a system view plugin or not
     * @return Boolean determining whether this plugin is a system view plugin or not
     */
    bool isSystemViewPlugin() const;

    /**
     * Create screenshot from the view plugin
     * Base implementation grabs the widget and saves it in a user-chosen location
     */
    virtual void createScreenshot();

    /**
     * Get shortcut for triggering the plugin
     * @return Shortcut key sequence
     */
    QKeySequence getTriggerShortcut() const;

    /**
     * Set shortcut for triggering the plugin
     * @param keySequence Shortcut key sequence
     */
    void setTriggerShortcut(const QKeySequence& keySequence);

public: // Title bar settings menu

    /**
     * Add action to the title bar menu
     * @param action Pointer to widget action
     */
    void addTitleBarMenuAction(gui::WidgetAction* action);

    /**
     * Remove action from the title bar menu
     * @param action Pointer to widget action
     */
    void removeTitleBarMenuAction(gui::WidgetAction* action);

    /**
     * Get title bar actions
     * @return List of pointers to title bar actions
     */
    gui::WidgetActions getTitleBarMenuActions();

public: // Settings actions

    /**
     * Add a docking action to the view (the action widget will be docked in the view)
     * @param dockingAction Pointer to docking action to add
     * @param dockToDockingAction Pointer to the settings action to dock to (docked top-level if nullptr)
     * @param dockArea Dock area in which \p dockToViewPlugin will be docked
     * @param autoHide Whether to pin the settings action to the border of the view plugin
     * @param autoHideLocation Location to pin the settings action in case auto-hide is active
     * @param minimumDockWidgetSize Minimum dock widget size
     */
    void addDockingAction(WidgetAction* dockingAction, WidgetAction* dockToDockingAction = nullptr, gui::DockAreaFlag dockArea = gui::DockAreaFlag::Right, bool autoHide = false, const gui::AutoHideLocation& autoHideLocation = gui::AutoHideLocation::Left, const QSize& minimumDockWidgetSize = QSize(256, 256));

    /** Get vector of pointers to docking actions */
    gui::WidgetActions getDockingActions() const;

public: // Overlay actions

	/**
     * Add \p overlayAction to the view (the action widget will be overlaid on top of the view)
     * @param overlayAction Pointer to overlay action to add (may not be nullptr)
     * @param alignment Alignment of the overlay action in the view
     */
    void addOverlayAction(WidgetAction* overlayAction, const Qt::Alignment& alignment = Qt::AlignTop | Qt::AlignLeft);

    /**
     * Remove \p overlayAction from the view
     * @param overlayAction Pointer to overlay action to remove (may not be nullptr)
     */
    void removeOverlayAction(WidgetAction* overlayAction);

	/**
     * Get all overlay actions
     * @return List of pointers to overlay actions
     */
    gui::WidgetActions getOverlayActions() const;

public: // Progress

    /**
     * Get progress task
     * @return Pointer to progress task (maybe nullptr)
     */
    Task* getProgressTask();

    /**
     * Set progress task to \p progressTask
     * @param progressTask Pointer to progress task (maybe nullptr)
     */
    void setProgressTask(Task* progressTask);

public: // Serialization

    /**
     * Load view plugin from variant
     * @param variantMap Variant map representation of the view plugin
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save view plugin to variant
     * @return Variant representation of the view plugin
     */
    QVariantMap toVariantMap() const override;

public: // Action getters

    gui::TriggerAction& getEditorAction() { return _editorAction; }
    gui::TriggerAction& getScreenshotAction() { return _screenshotAction; }
    gui::ToggleAction& getIsolateAction() { return _isolateAction; }
    gui::ToggleAction& getMayCloseAction() { return _mayCloseAction; }
    gui::ToggleAction& getMayFloatAction() { return _mayFloatAction; }
    gui::ToggleAction& getMayMoveAction() { return _mayMoveAction; }
    gui::OptionsAction& getDockingOptionsAction() { return _dockingOptionsAction; }
    gui::LockingAction& getLockingAction() { return _lockingAction; }
    gui::ToggleAction& getVisibleAction() { return _visibleAction; }
    gui::PresetsAction& getPresetsAction() { return _presetsAction; }
    gui::ViewPluginSamplerAction& getSamplerAction() { return _samplerAction; }

signals:

    /**
     * Signals that the progress task changed to \p progressTask
     * @param progressTask Pointer to progress task (maybe nullptr)
     */
    void progressTaskChanged(Task* progressTask);

    /**
     * Signals that \p action was added as a docking action
     * @param action Pointer to the added docking action
     */
    void dockingActionAdded(gui::WidgetAction* action);

private:
    QWidget                                 _widget;                            /** Widget representation of the plugin */
    gui::TriggerAction                      _editorAction;                      /** Trigger action to start the view plugin editor */
    gui::TriggerAction                      _screenshotAction;                  /** Trigger action to create a screenshot */
    gui::ToggleAction                       _isolateAction;                     /** Toggle action to setEnabled view isolation (when toggled, all other view plugins are temporarily closed) */
    gui::ToggleAction                       _mayCloseAction;                    /** Action for toggling whether the view plugin may be closed */
    gui::ToggleAction                       _mayFloatAction;                    /** Action for toggling whether the view plugin may float */
    gui::ToggleAction                       _mayMoveAction;                     /** Action for toggling whether the view plugin may be moved */
    gui::OptionsAction                      _dockingOptionsAction;              /** Action for toggling docking options */
    gui::LockingAction                      _lockingAction;                     /** Action for toggling whether the view plugin is locked */
    gui::ToggleAction                       _visibleAction;                     /** Action which determines whether the view plugin is visible or not */
    gui::PresetsAction                      _presetsAction;                     /** Action for managing presets */
    gui::ViewPluginSamplerAction            _samplerAction;                     /** Action for displaying a tooltip for sampled elements */
    QKeySequence                            _triggerShortcut;                   /** Shortcut for triggering the plugin */
    gui::WidgetActions                      _titleBarMenuActions;               /** Additional actions which are added to the end of the settings menu of the view plugin title bar */
    gui::WidgetActions                      _settingsActions;                   /** Settings actions which are displayed as docking widgets in the interface */
    Task*                                   _progressTask;                      /** When set and running, a thin progress bar will be displayed on top of the view plugin dock widget */
    std::vector<ActionWidgetPair>           _actionsWidgets;                    /** Pairs of overlay actions and their corresponding overlay widgets */
    gui::ViewPluginHeadsUpDisplayAction     _viewPluginHeadsUpDisplayAction;    /** Heads-up display action for displaying information as an overlay in the view plugin */
};

class CORE_EXPORT ViewPluginFactory : public PluginFactory
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param producesSystemViewPlugins Whether this factory generates system view plugins or not
     * @param startFloating Whether the view plugin is initially floating rather than docker to other view plugins
     */
    ViewPluginFactory(bool producesSystemViewPlugins = false, bool startFloating = false);

    /** Destructor */
    ~ViewPluginFactory() override = default;

    /**
     * Produces the plugin
     * @return Pointer to the produced plugin
     */
    ViewPlugin* produce() override = 0;

    /**
     * Get whether this factory produces system view plugins or not
     * @return Boolean determining whether this factory produces system view plugins or not
     */
    bool producesSystemViewPlugins() const;

    /**
     * Get preferred dock area
     * @return Preferred initial dock area when the view plugin is added to the workspace
     */
    gui::DockAreaFlag getPreferredDockArea() const;

    /**
     * Get whether the view plugin is initially floating (not docked to other view plugins)
     * @return Boolean determining whether the view plugin initially floats
     */
    bool getStartFloating() const;

protected:

    /**
     * Set preferred dock area
     * @param preferredDockArea Preferred initial dock area when the view plugin is added to the workspace
     */
    void setPreferredDockArea(const gui::DockAreaFlag& preferredDockArea);

private:
    const bool          _producesSystemViewPlugins;     /** Whether this factory produces system view plugins or not */
    gui::DockAreaFlag   _preferredDockArea;             /** Preferred initial dock area when the view plugin is added to the workspace */
    const bool          _startFloating;                 /** Whether the view plugin is initially floating rather than docker to other view plugins */
};

}

Q_DECLARE_INTERFACE(mv::plugin::ViewPluginFactory, "ManiVault.ViewPluginFactory")
