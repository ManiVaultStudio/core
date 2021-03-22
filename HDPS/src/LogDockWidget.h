#ifndef HDPS_LOGDOCKWIDGET_H
#define HDPS_LOGDOCKWIDGET_H

#include "widgets/DockableWidget.h"

// Standard C++ header file:
#include <memory> // For std::unique_ptr

namespace hdps
{
namespace gui
{

class LogDockWidget final: public DockableWidget
{
public:
    explicit LogDockWidget(const DockingLocation& location = DockingLocation::Left, QWidget* parent = nullptr);
    ~LogDockWidget() override;

private:
    class Data;
    const std::unique_ptr<const Data> _data;
};

} // namespace gui
} // namespace hdps

#endif // HDPS_LOGDOCKWIDGET_H
