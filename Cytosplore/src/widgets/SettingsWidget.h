#ifndef SETTINGS_WIDGET_H
#define SETTINGS_WIDGET_H

#include "DockableWidget.h"

#include <QWidget>
#include <QGridLayout>

namespace hdps
{
namespace gui
{

class SettingsWidget : public DockableWidget
{
    Q_OBJECT
public:
    void addWidget(QWidget* widget) {
        mainLayout()->addWidget(widget);
    }
};

} // namespace gui

} // namespace hdps

#endif // SETTINGS_WIDGET_H
