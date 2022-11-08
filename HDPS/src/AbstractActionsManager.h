#pragma once

#include "actions/WidgetAction.h"

#include <QObject>
#include <QAbstractItemModel>

class QSortFilterProxyModel;

namespace hdps
{

class AbstractActionsManager : public gui::WidgetAction
{
public:

    /**
     * Add action to the manager
     * @param action Pointer to action
     */
    virtual void addAction(WidgetAction* action) = 0;

    /**
     * Remove action from the manager
     * @param action Pointer to action
     */
    virtual void removeAction(WidgetAction* action) = 0;

    /**
     * Get actions model
     * @return List model for actions
     */
    virtual const QAbstractItemModel& getModel() const = 0;

    /**
     * Establish whether an action is public
     * @param action Action to test
     * @return Boolean indicating whether an action is public or not
     */
    virtual bool isActionPublic(const WidgetAction* action) const = 0;

    /**
     * Establish whether an action is published as a public action
     * @param action Action to test
     * @return Boolean indicating whether an action is published as a public action or not
     */
    virtual bool isActionPublished(const WidgetAction* action) const = 0;

    /**
     * Establish whether an action is connected to a public action
     * @param action Action to test
     * @return Boolean indicating whether an action is connected to a public action or not
     */
    virtual bool isActionConnected(const WidgetAction* action) const = 0;
};

}
