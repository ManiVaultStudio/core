#pragma once

#include "GroupAction.h"

namespace hdps::gui {

/**
 * Horizontal group action class
 *
 * Convenience group action for displaying child actions horizontally
 *
 * @author Thomas Kroes
 */
class HorizontalGroupAction : public GroupAction
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param expanded Whether the group is initially expanded/collapsed
     */
    HorizontalGroupAction(QObject* parent, const QString& title, const bool& expanded = false);

    /**
     * Get type string
     * @return Widget action type in string format
     */
    QString getTypeString() const override;
};

}
