#pragma once

#include "actions/WidgetAction.h"

#include <QStandardItemModel>

namespace hdps
{

class ActionsModel : public QStandardItemModel
{
public:

    /** Action columns */
    enum Column {
        Name,                   /** Name of the public action */
        ID,                     /** Globally unique identifier of the action */
        Type,                   /** Action type string */
        IsPublic,               /** Whether the action is public or private */

        Count
    };

public:

    /**
     * Add \p action to the model
     * @param action Pointer to action to add
     */
    void addAction(gui::WidgetAction* action);

    /**
     * Remove \p action from the model
     * @param action Pointer to action to remove
     */
    void removeAction(gui::WidgetAction* action);
};

}
