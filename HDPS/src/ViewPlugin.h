#ifndef HDPS_VIEWPLUGIN_H
#define HDPS_VIEWPLUGIN_H
/**
* View.h
*
* Base plugin class for plugins that visualize data.
*/

#include "widgets/DockableWidget.h"
#include "widgets/ViewPluginEditorDialog.h"
#include "actions/TriggerAction.h"
#include "actions/ToggleAction.h"
#include "Plugin.h"

#include <QWidget>
#include <QGridLayout>

class QToolBar;

namespace hdps
{
namespace plugin
{

class ViewPlugin : public Plugin
{
    Q_OBJECT
    
public:
    ViewPlugin(const PluginFactory* factory);

    ~ViewPlugin() override {};

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
    QWidget& getWidget()
    {
        return _widget;
    }

public: // Action getters

    gui::TriggerAction& getEditActionsAction() { return _editActionsAction; }
    gui::ToggleAction& getMayCloseAction() { return _mayCloseAction; }

private:
    QWidget                 _widget;                /** Widget representation of the plugin */
    gui::TriggerAction      _editActionsAction;     /** Trigger action to start editing the view plugin action hierarchy */
    gui::ToggleAction       _mayCloseAction;        /** Action for toggling whether a view plugin may be closed */
};

class ViewPluginFactory : public PluginFactory
{
    Q_OBJECT

public:
    ViewPluginFactory();

    ~ViewPluginFactory() override {};

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
};

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::ViewPluginFactory, "hdps.ViewPluginFactory")

#endif // HDPS_VIEWPLUGIN_H
