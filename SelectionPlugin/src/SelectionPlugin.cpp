#include "SelectionPlugin.h"

#include <QtCore>
#include <QtDebug>

Q_PLUGIN_METADATA(IID "nl.tudelft.SelectionPlugin")

// =============================================================================
// View
// =============================================================================

SelectionPlugin::~SelectionPlugin(void)
{
    
}

void SelectionPlugin::init()
{

}

// =============================================================================
// Factory
// =============================================================================

DataTypePlugin* SelectionPluginFactory::produce()
{
    return new SelectionPlugin();
}
