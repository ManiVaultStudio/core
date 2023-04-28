#pragma once

#include "ActionsModel.h"

namespace hdps
{

/**
 * Actions list model class
 *
 * List model for actions
 *
 * @author Thomas Kroes
 */
class ActionsListModel final : public ActionsModel
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent object and \p rootAction
     * @param parent Pointer to parent object
     * @param rootAction When set, only list the root action and its descendants
     */
    ActionsListModel(QObject* parent, gui::WidgetAction* rootAction = nullptr);

    /** Initializes the model from the current state of the actions manager */
    void initialize() override;

    /**
     * Add \p action to the model
     * @param action Pointer to action to add
     */
    void addAction(gui::WidgetAction* action) override;

    /**
     * Remove \p action from the model
     * @param action Pointer to action to remove
     */
    void removeAction(gui::WidgetAction* action) override;

private:
    gui::WidgetAction*      _rootAction;    /** When set, only list the root action and its descendants */
};

}
