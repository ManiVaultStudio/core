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
     * @param script Smart pointer to the script that should be executed when the trigger action is triggered
     * @param title Title of the plugin trigger action
     * @param menuLocation Determines where the script trigger action resides w.r.t. other script trigger actions (for instance in the data hierarchy context menu) in a path like fashion e.g. import/images
     * @param icon Icon representing the action (as shown in the menu)
     */
    Q_INVOKABLE ScriptTriggerAction(QObject* parent, const std::shared_ptr<util::Script>& script, const QString& menuLocation, const QIcon& icon = QIcon());

    /**
     * Get menu location
     * @return Path-separated menu location
     */
    QString getMenuLocation() const;

    /**
     * Get icon representing the scripting language
     * @return Icon representing the scripting language
     */
    QIcon getLanguageIcon() const;

private:

    /** Run the script (invoked when the trigger action is triggered) */
    void runScript() const;

private:
    std::shared_ptr<util::Script>   _script;            /** Smart pointer to the script that should be executed when the trigger action is triggered */
    QString                         _menuLocation;      /** Determines where the script trigger action resides w.r.t. other scrpt trigger actions (for instance in the data hierarchy context menu) in a path like fashion e.g. import/images */

    friend class plugin::PluginFactory;
};

using ScriptTriggerActions = QVector<QPointer<ScriptTriggerAction>>;

}

Q_DECLARE_METATYPE(mv::gui::ScriptTriggerAction)

inline const auto scriptTriggerActionMetaTypeId = qRegisterMetaType<mv::gui::ScriptTriggerAction*>("ScriptTriggerAction");