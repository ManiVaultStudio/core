#pragma once

#include "AbstractActionsManager.h"

#include <models/ActionsModel.h>

namespace hdps
{

using namespace gui;

class ActionsManager final : public AbstractActionsManager
{
    Q_OBJECT

public:

    /** Default constructor */
    ActionsManager();

    /** Default destructor */
    ~ActionsManager() override;

    /** Resets the contents of the actions manager */
    void reset() override;

    /**
     * Add action to the model
     * @param action Pointer to action
     */
    void addAction(WidgetAction* action) override;

    /**
     * Remove action from the model
     * @param action Pointer to action
     */
    void removeAction(WidgetAction* action) override;

    /**
     * Get actions model
     * @return List model for actions
     */
    const QAbstractItemModel& getModel() const override;

    /**
     * Establish whether an action is public
     * @param action Action to test
     * @return Boolean indicating whether an action is public or not
     */
    bool isActionPublic(const WidgetAction* action) const override;

    /**
     * Establish whether an action is published as a public action
     * @param action Action to test
     * @return Boolean indicating whether an action is published as a public action or not
     */
    bool isActionPublished(const WidgetAction* action) const override;

    /**
     * Establish whether an action is connected to a public action
     * @param action Action to test
     * @return Boolean indicating whether an action is connected to a public action or not
     */
    bool isActionConnected(const WidgetAction* action) const override;

private:
    ActionsModel        _model;         /** List model for actions */
};

}
