#include "DockComponentsFactory.h"

ads::CDockAreaTitleBar* DockComponentsFactory::createDockAreaTitleBar(ads::CDockAreaWidget* dockAreaWidget) const
{
    return new DockAreaTitleBar(dockAreaWidget);
}
