#pragma once

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
