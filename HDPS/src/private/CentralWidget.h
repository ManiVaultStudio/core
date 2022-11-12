#pragma once

#include "LogoWidget.h"

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

private:
    LogoWidget      _logoWidget;        /** Logo widget */
};
