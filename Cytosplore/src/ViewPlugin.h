#ifndef HDPS_PLUGIN_VIEW_PLUGIN
#define HDPS_PLUGIN_VIEW_PLUGIN

#include "widgets/DockableWidget.h"
#include "PluginFactory.h"
#include "DataConsumer.h"

#include <QWidget>
#include <QGridLayout>
#include <QComboBox>

namespace hdps {

namespace plugin {

class ViewPlugin : public gui::DockableWidget, public Plugin, public DataConsumer
{
    Q_OBJECT
    
public:
    ViewPlugin(QString name) : Plugin(Type::VIEW, name) {
        connect(&dataOptions, SIGNAL(currentIndexChanged(QString)), SLOT(dataSetPicked(QString)));
        addWidget(&dataOptions);
    }

    virtual ~ViewPlugin() {};

    void addData(QString name) {
        dataOptions.addItem(name);
    }

    void addWidget(QWidget* widget) {
        mainLayout()->addWidget(widget);
    }

protected:
    QComboBox dataOptions;

protected slots:
    virtual void dataSetPicked(const QString& name) = 0;
};

class ViewPluginFactory : public PluginFactory
{
    Q_OBJECT

public:

    virtual ~ViewPluginFactory() {};

    /**
     * Produces an instance of a view plugin. This function gets called by the plugin manager.
     */
    virtual ViewPlugin* produce() = 0;
};

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::ViewPluginFactory, "cytosplore.ViewPluginFactory")

#endif // HDPS_PLUGIN_VIEW_PLUGIN
