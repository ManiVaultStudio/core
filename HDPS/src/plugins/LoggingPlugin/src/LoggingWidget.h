#pragma once

#include <actions/GroupsAction.h>

#include <QWidget>

using namespace hdps;
using namespace hdps::util;
using namespace hdps::gui;

/**
 * Logging widget class
 *
 * Widget class for inspecting HDPS logs
 *
 * @author Thomas Kroes
 */
class LoggingWidget : public QWidget
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     */
    LoggingWidget(QWidget* parent);

private:
};
