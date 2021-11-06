#ifndef HDPS_VIEWPLUGIN_H
#define HDPS_VIEWPLUGIN_H
/**
* View.h
*
* Base plugin class for plugins that visualize data.
*/

#include "widgets/DockableWidget.h"
#include "Plugin.h"

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
    ViewPlugin(const PluginFactory* factory) :
        Plugin(factory)
    {
        setObjectName(getGuiName());
    }

    ~ViewPlugin() override {};

    /** Returns the toolbar which is shown at the top of the view plugin (dock) widget */
    virtual QToolBar* getToolBar() {
        return nullptr;
    };

public: // Actions

    /**
     * Add a (widget) action
     * This class does not alter the ownership of the allocated widget action
     * @param widgetAction Widget action to expose
     */
    void addAction(QAction* action) {
        Q_ASSERT(action != nullptr);

        QWidget::addAction(action);

        auto widgetAction = dynamic_cast<gui::WidgetAction*>(action);

        if (widgetAction == nullptr)
            return;

        widgetAction->setContext(_guiName);

        //Application::current()->getWidgetActionsManager().addAction(widgetAction);
    }
};

class ViewPluginFactory : public PluginFactory
{
    Q_OBJECT

public:
    ViewPluginFactory() :
        PluginFactory(Type::VIEW)
    {

    }
    ~ViewPluginFactory() override {};

    /** Returns the plugin icon */
    QIcon getIcon() const override {
        return Application::getIconFont("FontAwesome").getIcon("eye");
    }

    /**
     * Produces an instance of a view plugin. This function gets called by the plugin manager.
     */
    ViewPlugin* produce() override = 0;
};

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::ViewPluginFactory, "hdps.ViewPluginFactory")

#endif // HDPS_VIEWPLUGIN_H
