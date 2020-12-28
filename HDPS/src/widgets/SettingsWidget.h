#pragma once

#include "DockableWidget.h"

#include <QIcon>

namespace hdps
{
namespace gui
{

/**
 * Settings widget class
 *
 * @author Thomas Kroes
 */
class SettingsWidget : public DockableWidget
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Parent widget
     */
    SettingsWidget(QWidget* parent = nullptr);

public: // 

    /**
     * Add widget to the layout
     * @param widget Widget to add
     */
    void addWidget(QWidget* widget);
};

}
}