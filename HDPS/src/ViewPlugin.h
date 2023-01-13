#pragma once

#include "util/DockArea.h"

#include "actions/TriggerAction.h"
#include "actions/ToggleAction.h"
#include "actions/OptionsAction.h"
#include "actions/OptionAction.h"

#include "Plugin.h"

namespace hdps::gui {
    class ViewPluginTriggerAction;
}

namespace hdps::plugin
{

class ViewPlugin : public Plugin
{
    Q_OBJECT
    
public:

    /**
     * Constructor
     * @param factory Pointer to plugin factory
     */
    ViewPlugin(const PluginFactory* factory);

    /** Destructor */
    ~ViewPlugin() = default;

    /** Perform startup initialization */
    void init() override;

    /**
     * Set name of the object
     * @param name Name of the object
     */
    void setObjectName(const QString& name);

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
    virtual bool isSystemViewPlugin() const final;

    /**
     * Create screenshot from the view plugin
     * Base implementation grabs the widget and saves it in a user-chosen location
     */
    virtual void createScreenshot();

    /**
     * Get shortcut for triggering the plugin
     * @return Shortcut key sequence
     */
    virtual QKeySequence getTriggerShortcut() const final;

    /**
     * Set shortcut for triggering the plugin
     * @param keySequence Shortcut key sequence
     */
    virtual void setTriggerShortcut(const QKeySequence& keySequence) final;

public: // Serialization

    /**
     * Load view plugin from variant
     * @param Variant representation of the view plugin
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save view plugin to variant
     * @return Variant representation of the view plugin
     */
    QVariantMap toVariantMap() const override;

public: // Action getters

    gui::TriggerAction& getEditActionsAction() { return _editActionsAction; }
    gui::TriggerAction& getScreenshotAction() { return _screenshotAction; }
    gui::ToggleAction& getIsolateAction() { return _isolateAction; }
    gui::ToggleAction& getMayCloseAction() { return _mayCloseAction; }
    gui::ToggleAction& getMayFloatAction() { return _mayFloatAction; }
    gui::ToggleAction& getMayMoveAction() { return _mayMoveAction; }
    gui::ToggleAction& getVisibleAction() { return _visibleAction; }

private:
    QWidget                 _widget;                /** Widget representation of the plugin */
    gui::TriggerAction      _editActionsAction;     /** Trigger action to start editing the view plugin action hierarchy */
    gui::TriggerAction      _screenshotAction;      /** Trigger action to create a screenshot */
    gui::ToggleAction       _isolateAction;         /** Toggle action to toggle view isolation (when toggled, all other view plugins are temporarily closed) */
    gui::ToggleAction       _mayCloseAction;        /** Action for toggling whether a view plugin may be closed */
    gui::ToggleAction       _mayFloatAction;        /** Action for toggling whether a view plugin may float */
    gui::ToggleAction       _mayMoveAction;         /** Action for toggling whether a view plugin may be moved */
    gui::ToggleAction       _visibleAction;         /** Action which determines whether the view plugin is visible or not */
    gui::TriggerAction      _helpAction;            /** Action which triggers documentation */
    QKeySequence            _triggerShortcut;       /** Shortcut for triggering the plugin */
};

class ViewPluginFactory : public PluginFactory
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param producesSystemViewPlugins Whether this factory generates system view plugins or not
     */
    ViewPluginFactory(bool producesSystemViewPlugins = false);

    /** Destructor */
    ~ViewPluginFactory() = default;

    /**
     * Get plugin icon
     * @param color Icon color for flat (font) icons
     * @return Icon
     */
    QIcon getIcon(const QColor& color = Qt::black) const override;

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

protected:

    /**
     * Set preferred dock area
     * @param preferredDockArea Preferred initial dock area when the view plugin is added to the workspace
     */
    void setPreferredDockArea(const gui::DockAreaFlag& preferredDockArea);

private:
    const bool          _producesSystemViewPlugins;     /** Whether this factory produces system view plugins or not */
    gui::DockAreaFlag   _preferredDockArea;             /** Preferred initial dock area when the view plugin is added to the workspace */
};

}

Q_DECLARE_INTERFACE(hdps::plugin::ViewPluginFactory, "hdps.ViewPluginFactory")
