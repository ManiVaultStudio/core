// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ScriptTriggerAction.h"

namespace mv::gui {

ScriptTriggerAction::ScriptTriggerAction(QObject* parent, const util::Script::Type& type, const util::Script::Language& language, const QUrl& location, const Datasets& datasets, const QString& title, const QString& tooltip, const QIcon& icon) :
    TriggerAction(parent, title),
    _script(type, language, location, datasets)
{
    setText(title);
    setToolTip(tooltip);
    setIcon(icon);

    connect(this, &TriggerAction::triggered, this, &ScriptTriggerAction::runScript);
}

void ScriptTriggerAction::runScript()
{
    qDebug().noquote() << QString("Running %1 script (%2): %3").arg(util::Script::getTypeName(_script.getType()), util::Script::getLanguageName(_script.getLanguage()), _script.getLocation().toString());

    _script.run();
}

}
