#ifndef HDPS_LOGDOCKWIDGET_H
#define HDPS_LOGDOCKWIDGET_H

// Qt header file:
#include <QDockWidget>

// Standard C++ header file:
#include <memory> // For std::unique_ptr

namespace hdps
{
namespace gui
{

class LogDockWidget final: public QDockWidget
{
public:
    explicit LogDockWidget(QMainWindow&);
    ~LogDockWidget() override;

private:
    class Data;
    const std::unique_ptr<const Data> _data;
};

} // namespace gui
} // namespace hdps

#endif // HDPS_LOGDOCKWIDGET_H
