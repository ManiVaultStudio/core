// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ScriptTriggerAction.h"

namespace mv::gui {

ScriptTriggerAction::ScriptTriggerAction(QObject* parent, util::Script* script, const QString& title, const QString& tooltip, const QIcon& icon) :
    TriggerAction(parent, title),
    _script(script)
{
    setText(title);
    setToolTip(tooltip);
    setIcon(icon);

    connect(this, &TriggerAction::triggered, this, &ScriptTriggerAction::runScript);
}

void ScriptTriggerAction::runScript()
{
    if (_script) {
        _script->run();
    } else {
        qWarning() << "Unable to run script: script is not available!";
    }
}

}
