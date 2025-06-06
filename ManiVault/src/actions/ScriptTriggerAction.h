// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "TriggerAction.h"

#include "Dataset.h"

#include "util/Script.h"

namespace mv::gui {

/**
 * Script trigger action class
 *
 * Action class for triggering script execution
 * 
 * Note: This action is developed for internal use only
 * 
 * @author Thomas Kroes
 */
class CORE_EXPORT ScriptTriggerAction : public TriggerAction
{
    Q_OBJECT
    
	/** Request plugin callback function (invoked when the trigger action is triggered) */
    //using RunScriptCallback = std::function<void(ScriptTriggerAction&)>;

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param script Pointer to the script that should be executed when the trigger action is triggered
     * @param title Title of the plugin trigger action
     * @param icon Icon
     * @param tooltip Tooltip of the plugin trigger action
     */
    Q_INVOKABLE ScriptTriggerAction(QObject* parent, util::Script* script, const QString& title, const QString& tooltip, const QIcon& icon);

private:

    /** Run the script (invoked when the trigger action is triggered) */
    void runScript();

private:
    QPointer<util::Script>  _script;            /** Pointer to the script that should be executed when the trigger action is triggered */
    QString                 _menuLocation;      /** Determines where the script trigger action resides w.r.t. other scrpt trigger actions (for instance in the data hierarchy context menu) in a path like fashion e.g. import/images */

    friend class plugin::PluginFactory;
};

using ScriptTriggerActions = QVector<QPointer<ScriptTriggerAction>>;

}

Q_DECLARE_METATYPE(mv::gui::ScriptTriggerAction)

inline const auto scriptTriggerActionMetaTypeId = qRegisterMetaType<mv::gui::ScriptTriggerAction*>("ScriptTriggerAction");