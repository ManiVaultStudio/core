#pragma once

#include "DockWidget.h"

/**
 * Central dock widget class
 *
 * Widget class which is placed in the central area of the docking manager.
 *
 * @author Thomas Kroes
 */
class CentralDockWidget : public DockWidget
{
public:

    /**
     * Constructor
     * @param title Title of the dock widget
     * @param parent Pointer to parent widget
     */
    CentralDockWidget(QWidget* parent = nullptr);

    /**
     * Get string that describes the dock widget type
     * @return Type string
     */
    QString getTypeString() const override;
};
