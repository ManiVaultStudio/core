// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LoaderPlugin.h"

#include "actions/PluginTriggerAction.h"

#include "widgets/FileDialog.h"

#include <QSettings>
#include <QString>

using namespace mv::gui;

namespace mv::plugin {

QString LoaderPlugin::AskForFileName(const QString& fileNameFilter) const
{
    QSettings settings(QLatin1String{"ManiVault"}, QLatin1String{ "Plugins/" } + getKind());

    const QLatin1String directoryPathKey("directoryPath");

    const auto directoryPath    = settings.value(directoryPathKey).toString();
    const auto fileName         = FileDialog::getOpenFileName(nullptr, QObject::tr("Load File"), directoryPath, fileNameFilter + QObject::tr(";;All Files (*)"));

    // Only store the directory name when the dialog has not been not canceled and the file name is non-empty.
    if (!fileName.isEmpty())
        settings.setValue(directoryPathKey, QFileInfo(fileName).absolutePath());
    
    return fileName;
}

PluginTriggerActions LoaderPluginFactory::getPluginTriggerActions(const Datasets& datasets) const
{
    return { new PluginTriggerAction(const_cast<LoaderPluginFactory*>(this), this, QString("%1").arg(getKind()), QString("Load %1").arg(getKind()), getIcon()) };
}

PluginTriggerActions LoaderPluginFactory::getPluginTriggerActions(const DataTypes& dataTypes) const
{
    return getPluginTriggerActions(DataTypes({}));
}

}
