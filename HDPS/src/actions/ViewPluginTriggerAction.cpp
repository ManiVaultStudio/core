#include "ViewPluginTriggerAction.h"
#include "AbstractLayoutManager.h"

using namespace hdps::plugin;
using namespace hdps::gui;

namespace hdps::gui {

ViewPluginTriggerAction::ViewPluginTriggerAction(QObject* parent, const QString viewPluginKind) :
    PluginTriggerAction(parent, viewPluginKind, "View plugin trigger"),
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

//ViewPluginTriggerAction::ViewPluginTriggerAction(QObject* parent, const QString& viewPluginKind, ViewPlugin* dockViewPlugin, ViewPlugin::DockArea dockArea) :
//    PluginTriggerAction(parent, viewPluginKind, "View plugin trigger"),
//    _dockViewPlugin(),
//    _dockArea()
//{
//    //
//

//
//    //const auto getProducePluginTriggerAction = [&](const QString& guiName) -> PluginTriggerAction& {
//    //    pluginFactory->setGuiName(guiName);
//
//    //    auto& producePluginTriggerAction = pluginFactory->getProducePluginTriggerAction();
//
//    //    producePluginTriggerAction.setIcon(pluginFactory->getIcon());
//    //    pluginFactory->getTriggerHelpAction().setIcon(pluginFactory->getIcon());
//
//    //    connect(&producePluginTriggerAction, &PluginTriggerAction::triggered, this, [this, pluginFactory]() -> void {
//    //        createPlugin(pluginFactory->getKind());
//    //        });
//
//    //    return producePluginTriggerAction;
//    //};
//}

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
        qDebug() << getPluginFactory()->getKind() << "triggered";

        auto viewPlugin = Application::core()->requestPlugin(getPluginFactory()->getKind(), _dockToViewPlugin, _dockArea);

        Application::core()->getLayoutManager().addViewPlugin(dynamic_cast<ViewPlugin*>(viewPlugin));
    });
}

}