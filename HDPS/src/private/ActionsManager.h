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
     * Add action to the actions model
     * @param action Pointer to action
     */
    void addActionToModel(gui::WidgetAction* action) override;

    /**
     * Remove action from the actions model
     * @param action Pointer to action
     */
    void removeActionFromModel(gui::WidgetAction* action) override;

    /**
     * Publish \p privateAction so that other private actions can connect to it
     * @param privateAction Pointer to private action to publish
     * @param name Name of the published widget action (if empty, a name choosing dialog will popup)
     */
    void publishPrivateAction(gui::WidgetAction* privateAction, const QString& name = "") override;

public: // Model

    /**
     * Get model which contains all actions
     * @return Reference to actions model
     */
    ActionsModel& getActionsModel() override;

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
    ActionsModel    _model;     /** Actions model */
};

}
