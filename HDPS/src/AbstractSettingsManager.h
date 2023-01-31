#pragma once

#include "AbstractManager.h"

#include "actions/TriggerAction.h"
#include "actions/DirectoryPickerAction.h"

namespace hdps
{

/**
 * Abstract settings manager
 *
 * Base abstract settings manager class for managing global settings.
 *
 * @author Thomas Kroes
 */
class AbstractSettingsManager : public AbstractManager
{
    Q_OBJECT

public:

    /**
     * Construct settings manager with \p parent object
     * @param parent Pointer to parent object
     */
    AbstractSettingsManager(QObject* parent = nullptr) :
        AbstractManager(parent, "Settings")
    {
    }

    /** Opens the settings editor dialog */
    virtual void edit() = 0;

public: // Action getters

    virtual gui::TriggerAction& getEditSettingsAction() = 0;
    virtual gui::DirectoryPickerAction& getGlobalProjectsPathAction() = 0;
    virtual gui::DirectoryPickerAction& getGlobalWorkspacesPathAction() = 0;
    virtual gui::DirectoryPickerAction& getGlobalDataPathAction() = 0;
};

}
