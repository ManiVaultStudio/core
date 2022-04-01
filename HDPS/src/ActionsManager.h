#pragma once

#include "ActionsModel.h"

#include <QObject>

namespace hdps
{

namespace gui {
    class WidgetAction;
}

using namespace gui;

class ActionsManager : public QObject
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    ActionsManager(QObject* parent = nullptr);

    /**
     * Add action to the model
     * @param action Pointer to action
     */
    void addAction(WidgetAction* action);

    /**
     * Remove action from the model
     * @param action Pointer to action
     */
    void removeAction(WidgetAction* action);

    /**
     * Get actions model
     * @return List model for actions
     */
    const ActionsModel& getActionsModel() const;

    /**
     * Establish whether an action is public
     * @param action Action to test
     * @return Boolean indicating whether an action is public or not
     */
    bool isActionPublic(const WidgetAction* action) const;

    /**
     * Establish whether an action is published as a public action
     * @param action Action to test
     * @return Boolean indicating whether an action is published as a public action or not
     */
    bool isActionPublished(const WidgetAction* action) const;

    /**
     * Establish whether an action is connected to a public action
     * @param action Action to test
     * @return Boolean indicating whether an action is connected to a public action or not
     */
    bool isActionConnected(const WidgetAction* action) const;

private:
    ActionsModel    _actionsModel;        /** List model for actions */
};

}
