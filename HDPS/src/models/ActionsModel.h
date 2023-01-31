#pragma once

#include "actions/WidgetAction.h"

#include <QStandardItemModel>

namespace hdps
{

class ActionsModel : public QStandardItemModel
{
public:

    /** Action columns */
    enum class Column {
        Name,       /** Name of the action */
        ID,         /** Globally unique identifier of the action */
        Type,       /** Action type string */
        Scope,      /** Scope of the action (whether the action is public or private) */

        Count
    };

public:

    /**
     * Construct with \p parent object
     * @param parent Pointer to parent object
     */
    ActionsModel(QObject* parent = nullptr);

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
