// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ScriptTriggerAction.h"

namespace mv::gui {

ScriptTriggerAction::ScriptTriggerAction(QObject* parent, const std::shared_ptr<util::Script>& script, const QString& menuLocation, const QIcon& icon /*= QIcon()*/) :
    TriggerAction(parent, script ? script->getTitle() : ""),
    _script(script),
    _menuLocation(QString("%1 %2/%3").arg(util::Script::getLanguageName(script ? script->getLanguage() : util::Script::Language::None), script ? QString::fromStdString(script->getLanguageVersion().getVersionString()) : QString(), menuLocation))
{
    setIcon(icon);

    connect(this, &TriggerAction::triggered, this, &ScriptTriggerAction::runScript);
}

QString ScriptTriggerAction::getMenuLocation() const
{
	return _menuLocation;
}

QIcon ScriptTriggerAction::getLanguageIcon() const
{
    if (_script)
		return _script->getLanguageIcon();

    return {};
}

void ScriptTriggerAction::runScript() const
{
    if (!_script) {
        qWarning() << "ScriptTriggerAction::runScript: No script is set for this action.";
        return;
    }

    _script->run();
}

}
