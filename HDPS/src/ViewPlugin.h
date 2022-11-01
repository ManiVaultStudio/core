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
    ViewPlugin(const PluginFactory* factory) :
        Plugin(factory),
        _widget(),
        _editActionsAction(&_widget, "Edit view plugin actions")
    {
        setText(getGuiName());

        _widget.addAction(&_editActionsAction);

        _editActionsAction.setShortcut(tr("F12"));
        _editActionsAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);

        connect(&_editActionsAction, &TriggerAction::triggered, this, [this]() -> void {
            ViewPluginEditorDialog viewPluginEditorDialog(nullptr, this);
            viewPluginEditorDialog.exec();
        });
    }

    ~ViewPlugin() override {};

    /**
     * Set name of the object
     * @param name Name of the object
     */
    void setObjectName(const QString& name)
    {
        QObject::setObjectName("Plugins/View/" + name);
    }

    /** Returns the toolbar which is shown at the top of the view plugin (dock) widget */
    virtual QToolBar* getToolBar() {
        return nullptr;
    };

    /**
     * Load one (or more) datasets in the view
     * @param datasets Dataset(s) to load
     */
    virtual void loadData(const Datasets& datasets) {
        qDebug() << "Load function not implemented in view plugin implementation";
    }

    /**
     * Get widget representation of the plugin
     * @return Widget representation of the plugin
     */
    QWidget& getWidget()
    {
        return _widget;
    }

protected:
    QWidget             _widget;                /** Widget representation of the plugin */
    gui::TriggerAction  _editActionsAction;     /** Trigger action to start editing the view plugin action hierarchy */
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

    /**
     * Get plugin icon
     * @param color Icon color for flat (font) icons
     * @return Icon
     */
    QIcon getIcon(const QColor& color = Qt::black) const override {
        return Application::getIconFont("FontAwesome").getIcon("eye", color);
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
