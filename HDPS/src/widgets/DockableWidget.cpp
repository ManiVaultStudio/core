#include "DockableWidget.h"

namespace hdps
{
namespace gui
{

DockableWidget::DockableWidget(const DockingLocation& location /*= Location::Left*/, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _dockingLocation(location)
{
}

DockableWidget::~DockableWidget()
{
}

hdps::gui::DockableWidget::DockingLocation DockableWidget::getDockingLocation() const
{
    return _dockingLocation;
}

void DockableWidget::setDockingLocation(const DockingLocation& location)
{
    _dockingLocation = location;
}

}
}