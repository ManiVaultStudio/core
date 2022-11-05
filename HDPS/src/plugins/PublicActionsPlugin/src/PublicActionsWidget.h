#pragma once

#include <QWidget>

namespace hdps
{

namespace gui
{

/**
 * Public actions widget class
 *
 * Widget class for viewing/editing public actions
 *
 * @author Thomas Kroes
 */
class PublicActionsWidget : public QWidget
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     */
    PublicActionsWidget(QWidget* parent);
};

}
}
