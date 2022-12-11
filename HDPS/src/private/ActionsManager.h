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

private:
    ActionsModel        _model;         /** List model for actions */
};

}
