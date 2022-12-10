#pragma once

#include <actions/GroupsAction.h>

/**
 * Global settings action class
 *
 * Action class for modifying global settings
 *
 * @author Thomas Kroes
 */
class GlobalSettingsAction : public hdps::gui::GroupsAction
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent object
     * @param parent Pointer to parent object
     */
    GlobalSettingsAction(QObject* parent);
};
