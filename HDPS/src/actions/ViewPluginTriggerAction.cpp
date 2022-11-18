#include "ViewPluginTriggerAction.h"
#include "AbstractLayoutManager.h"

using namespace hdps::plugin;
using namespace hdps::gui;

namespace hdps::gui {

ViewPluginTriggerAction::ViewPluginTriggerAction(QObject* parent, const ViewPluginFactory* viewPluginFactory) :
    PluginTriggerAction(parent, viewPluginFactory, "View plugin trigger"),
    _dockToViewPlugin(nullptr),
    _dockArea(gui::DockAreaFlag::Right)
{
}

ViewPluginTriggerAction::ViewPluginTriggerAction(QObject* parent, const QString viewPluginKind, plugin::ViewPlugin* dockViewPlugin, gui::DockAreaFlag dockArea) :
    PluginTriggerAction(parent, viewPluginKind, "View plugin trigger"),
    _dockToViewPlugin(dockViewPlugin),
    _dockArea(dockArea)
{
}

ViewPlugin* ViewPluginTriggerAction::getDockToViewPlugin() const
{
    return _dockToViewPlugin;
}

void ViewPluginTriggerAction::setDockToViewPlugin(ViewPlugin* dockToViewPlugin)
{
    _dockToViewPlugin = dockToViewPlugin;
}

DockAreaFlag ViewPluginTriggerAction::getDockArea() const
{
    return _dockArea;
}

void ViewPluginTriggerAction::setDockArea(gui::DockAreaFlag dockArea)
{
    _dockArea = dockArea;
}

void ViewPluginTriggerAction::initialize()
{
    PluginTriggerAction::initialize();

    setText(getPluginFactory()->getKind());

    connect(this, &PluginTriggerAction::triggered, this, [this]() -> void {
        

        qDebug() << "View plugin triggered" << getPluginFactory()->getKind() << gui::dockAreaMap.key(_dockArea);

        
    });
}

void ViewPluginTriggerAction::trigger(plugin::ViewPlugin* dockViewPlugin, gui::DockAreaFlag dockArea)
{
    auto viewPlugin = Application::core()->requestPlugin(getPluginFactory()->getKind());
    Application::core()->getLayoutManager().addViewPlugin(dynamic_cast<ViewPlugin*>(viewPlugin), _dockToViewPlugin, _dockArea);
}

}