#ifndef HDPS_VIEWPLUGIN_H
#define HDPS_VIEWPLUGIN_H
/**
* View.h
*
* Base plugin class for plugins that visualize data.
*/

#include "widgets/DockableWidget.h"
#include "PluginFactory.h"

#include <QWidget>
#include <QGridLayout>

class QToolBar;

namespace hdps
{
namespace plugin
{

class ViewPlugin : public gui::DockableWidget, public Plugin
{
    Q_OBJECT
    
public:
    ViewPlugin(QString name) :
        Plugin(Type::VIEW, name)
    {
        setObjectName(getGuiName());
    }

    ~ViewPlugin() override {};

    /** Returns the icon of this plugin */
    QIcon getIcon() const override {
        return Application::getIconFont("FontAwesome").getIcon("binoculars");
    }

    /** Returns the toolbar which is shown at the top of the view plugin (dock) widget */
    virtual QToolBar* getToolBar() {
        return nullptr;
    };
};

class ViewPluginFactory : public PluginFactory
{
    Q_OBJECT

public:

    ~ViewPluginFactory() override {};

    /**
     * Produces an instance of a view plugin. This function gets called by the plugin manager.
     */
    ViewPlugin* produce() override = 0;
};

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::ViewPluginFactory, "cytosplore.ViewPluginFactory")

#endif // HDPS_VIEWPLUGIN_H
