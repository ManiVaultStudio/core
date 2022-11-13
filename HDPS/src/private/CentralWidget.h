#pragma once

#include "LogoWidget.h"

#include <DockManager.h>

#include <QWidget>

/**
 * Central widget class
 *
 * Widget class for the central dock widget
 *
 * @author Thomas Kroes
 */
class CentralWidget : public QWidget
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     */
    CentralWidget(QWidget* parent = nullptr);

    QSharedPointer<ads::CDockManager> getDockManager();

private:
    QWidget                         _widget;            
    QSharedPointer<ads::CDockManager>     _dockManager;       /** ADS inherited dock manager */
    //LogoWidget                      _logoWidget;        /** Logo widget */
};
