// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "Task.h"
#include "ProjectSerializationTask.h"

namespace mv {

/**
 * Application startup task class
 *
 * Defines a startup application task with all the necessary descendant tasks.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ApplicationStartupTask final : public Task
{
    Q_OBJECT

public:

    /**
    * Construct task with \p parent object, \p name and initial \p status
    * @param parent Pointer to parent object
    * @param name Name of the task
    * @param status Initial status of the task
    * @param mayKill Boolean determining whether the task may be killed or not
    */
    ApplicationStartupTask(QObject* parent, const QString& name, const Status& status = Status::Undefined, bool mayKill = false);

public: // Task getters

    Task& getLoadCoreTask();                            /** Get aggregate task for loading the core */
    Task& getLoadCoreManagersTask();                    /** Get aggregate task for loading the core managers */
    Task& getLoadGuiTask();                             /** Get task for loading the GUI */
    ProjectSerializationTask& getLoadProjectTask();     /** Get task for loading a project */

private:
    Task                        _loadCoreTask;              /** Aggregate task for loading the core */
    Task                        _loadCoreManagersTask;      /** Aggregate task for loading the core managers */
    Task                        _loadGuiTask;               /** Task for loading the GUI */
    ProjectSerializationTask    _loadProjectTask;           /** Task for possibly loading a project at application startup */
};

}
