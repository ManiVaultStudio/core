#pragma once

#include "AbstractActionsManager.h"

#include <models/ActionsModel.h>

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
    void initialize() override;

    /** Resets the contents of the actions manager */
    void reset() override;

    /**
     * Get all actions in the manager
     * @return List of all actions in the manager
     */
    const gui::WidgetActions& getActions() const override;

    /**
     * Get actions with \p id
     * @return Pointer to widget action (may return nullptr)
     */
    gui::WidgetAction* getAction(const QString& id) override;

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
     * Add public action to the manager
     * @param action Pointer to public action
     */
    void addPublicAction(gui::WidgetAction* action) override;

    /**
     * Remove public action from the manager
     * @param action Pointer to public action
     */
    void removePublicAction(gui::WidgetAction* action) override;

public: // Serialization

    /**
     * Load manager from variant map
     * @param Variant map representation of the manager
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save manager to variant map
     * @return Variant map representation of the manager
     */
    QVariantMap toVariantMap() const override;

private:
    gui::WidgetActions  _actions;           /** Keep track of allocated actions */
    gui::WidgetActions  _publicActions;     /** Keep track of allocated public actions */
};

}
