#pragma once

#include "widgets/DockableWidget.h"
#include "actions/TriggerAction.h"
#include "actions/ToggleAction.h"
#include "actions/OptionsAction.h"
#include "actions/OptionAction.h"

#include "Plugin.h"

#include <QWidget>
#include <QGridLayout>

namespace hdps
{
namespace plugin
{

class ViewPlugin : public Plugin
{
    Q_OBJECT
    
public:

    /** Maps dock widget area to dock widget area flag */
    static QMap<QString, std::uint32_t> dockWidgetAreaMap;

    /**
     * Get dock widget areas
     * @param dockWidgetAreas List of dock area strings
     * @return Dock areas
     */
    static std::int32_t getDockWidgetAreas(const QStringList& dockWidgetAreas);

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
     * Get whether this plugin is a standard (system) view plugin or not
     * @return Boolean determining whether this plugin is a standard (system) view plugin or not
     */
    virtual bool isStandardView() const final;

public: // Action getters

    gui::TriggerAction& getEditActionsAction() { return _editActionsAction; }
    gui::ToggleAction& getMayCloseAction() { return _mayCloseAction; }
    gui::ToggleAction& getMayFloatAction() { return _mayFloatAction; }
    gui::ToggleAction& getMayMoveAction() { return _mayMoveAction; }
    gui::ToggleAction& getVisibleAction() { return _visibleAction; }
    gui::OptionsAction& getAllowedDockingAreasAction() { return _allowedDockingAreasAction; }
    gui::OptionAction& getPreferredDockingAreaAction() { return _preferredDockingAreaAction; }

private:
    QWidget                 _widget;                        /** Widget representation of the plugin */
    gui::TriggerAction      _editActionsAction;             /** Trigger action to start editing the view plugin action hierarchy */
    gui::ToggleAction       _mayCloseAction;                /** Action for toggling whether a view plugin may be closed */
    gui::ToggleAction       _mayFloatAction;                /** Action for toggling whether a view plugin may float */
    gui::ToggleAction       _mayMoveAction;                 /** Action for toggling whether a view plugin may be moved */
    gui::ToggleAction       _visibleAction;                 /** Action which determines whether the view plugin is visible or not */
    gui::OptionsAction      _allowedDockingAreasAction;     /** Action which determines the allowed docking areas */
    gui::OptionAction       _preferredDockingAreaAction;    /** Action which determines the preferred docking area */
    gui::TriggerAction      _triggerHelpAction;             /** Action which shows help (internal use only) */
};

class ViewPluginFactory : public PluginFactory
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param isStandardView Whether this factory generates standard (system) view plugins or not
     */
    ViewPluginFactory(bool isStandardView = false);

    /** Destructor */
    ~ViewPluginFactory() = default;

    /**
     * Get plugin icon
     * @param color Icon color for flat (font) icons
     * @return Icon
     */
    QIcon getIcon(const QColor& color = Qt::black) const override;

    /**
     * Produces an instance of a view plugin. This function gets called by the plugin manager.
     */
    ViewPlugin* produce() override = 0;

    /**
     * Get whether this factory generates standard (system) view plugins or not
     * @return Boolean determining whether this factory generates standard (system) view plugins or not
     */
    bool isStandardView() const;

private:
    const bool      _isStandardView;        /** Whether this factory generates standard (system) view plugins or not */
};

}

}

Q_DECLARE_INTERFACE(hdps::plugin::ViewPluginFactory, "hdps.ViewPluginFactory")
