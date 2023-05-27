#pragma once

#include "models/AbstractActionsModel.h"

namespace hdps
{

/**
 * Actions hierarchy model class
 *
 * Hierarchical actions model class for management of public actions (parameters)
 *
 * @author Thomas Kroes
 */
class PublicActionsModel final : public AbstractActionsModel
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent object
     * @param parent Pointer to parent object
     */
    PublicActionsModel(QObject* parent);

    /** Initializes the model from the current state of the actions manager */
    void initialize() override;


    QStringList mimeTypes() const;

    bool dropMimeData(const QMimeData* mimeData, Qt::DropAction action, int row, int column, const QModelIndex& parent);

protected:

    /**
     * Helper method which is invoked when \p action is added to the actions manager
     * @param action Pointer to action that was added
     */
    void actionAddedToManager(gui::WidgetAction* action) override;

    /**
     * Helper method which is invoked when \p action is about to be removed from the actions manager
     * @param action Pointer to action that was removed
     */
    void actionAboutToBeRemovedFromManager(gui::WidgetAction* action) override;

    /**
     * Helper method which is invoked when \p publicAction is added to the actions manager
     * @param publicAction Pointer to public action that was added
     */
    void publicActionAddedToManager(gui::WidgetAction* publicAction) override;

    /**
     * Helper method which is invoked when \p publicAction is about to be removed from the actions manager
     * @param publicAction Pointer to public action to remove
     */
    void publicActionAboutToBeRemovedFromManager(gui::WidgetAction* publicAction) override;

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

    /**
     * Add \p publicAction to the model
     * @param publicAction Pointer to public action to add
     */
    void addPublicAction(gui::WidgetAction* publicAction);

    /**
     * Remove \p publicAction from the model
     * @param publicAction Pointer to public action to remove
     */
    void removePublicAction(gui::WidgetAction* publicAction);
};

}
