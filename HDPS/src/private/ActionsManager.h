#pragma once

#include "AbstractActionsManager.h"

#include <models/ActionsModel.h>

#include <QList>

namespace hdps
{

class ActionsManager final : public AbstractActionsManager
{
    Q_OBJECT

public:

    /** Default constructor */
    ActionsManager();

    /** Default destructor */
    ~ActionsManager() override;

    /** Perform manager startup initialization */
    void initalize() override;

    /** Resets the contents of the actions manager */
    void reset() override;

    /**
     * Add action to the model
     * @param action Pointer to action
     */
    void addAction(gui::WidgetAction* action) override;

    /**
     * Remove action from the model
     * @param action Pointer to action
     */
    void removeAction(gui::WidgetAction* action) override;

    /**
     * Get model which is associated with the manager
     * @return Reference to an abstract item model inherited model class
     */
    const QAbstractItemModel& getModel() const override;

private:
    gui::WidgetActions  _actions;   /** Keep track of allocated actions */
    ActionsModel        _model;     /** Actions model */
};

}
