#pragma once

#include "GroupAction.h"
#include "ToggleAction.h"
#include "TriggerAction.h"

namespace hdps::gui {

/**
 * Locked action class
 *
 * Action class for locking status (internal use only).
 *
 * @author Thomas Kroes
 */
class LockingAction final : public GroupAction
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param locked Locked
     */
    LockingAction(QObject* parent, bool locked = false);

    /**
     * Get type string
     * @return Widget action type in string format
     */
    QString getTypeString() const override;

    /**
     * Initialize the toggle action
     * @param locked Locked
     */
    void initialize(bool locked = false);

    /**
     * Get locked status
     * @return Boolean determining the locked status
     */
    bool isLocked() const;

    /**
     * Set locked status to \p locked
     * @param locked Boolean determining the locked status
     */
    void setLocked(bool locked);

public: // Serialization

    /**
     * Load locked action from variant map
     * @param Variant map representation of the locked action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
      * Save locked action to variant map
      * @return Variant map representation of the locked action
      */
    QVariantMap toVariantMap() const override;

public: // Action getters

    gui::ToggleAction& getLockedAction() { return _lockedAction; }
    gui::TriggerAction& getLockAction() { return _lockAction; }
    gui::TriggerAction& getUnlockAction() { return _unlockAction; }

signals:

    /**
     * Signals that the locked status changed
     * @param locked Locked status
     */
    void lockedChanged(bool locked);

private:
    gui::ToggleAction   _lockedAction;      /** Action for toggling the locking status */
    gui::TriggerAction  _lockAction;        /** Action for locking */
    gui::TriggerAction  _unlockAction;      /** Action for unlocking */
};

}
