// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ScriptTriggerAction.h"

namespace mv::gui {

ScriptTriggerAction::ScriptTriggerAction(QObject* parent, const util::Script::Type& type, const util::Script::Language& language, const QUrl& location, const Datasets& datasets, const QString& title, const QString& menuLocation, QIcon icon /*= QIcon()*/) :
    TriggerAction(parent, title),
    _script(title, type, language, location, datasets),
    _menuLocation(QString("%1/%2").arg(util::Script::getLanguageName(language), menuLocation))
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
    return _script.getLanguageIcon();
}

void ScriptTriggerAction::runScript()
{
    qDebug().noquote() << QString("Running %1 script %2 (%3): %4").arg(util::Script::getTypeName(_script.getType()), _script.getTitle(), util::Script::getLanguageName(_script.getLanguage()), _script.getLocation().toString());

    _script.run();
}

}
