#include "LoaderPlugin.h"

#include <actions/PluginTriggerAction.h>

#include <QFileDialog>
#include <QSettings>
#include <QString>

using namespace hdps::gui;

namespace hdps {

namespace plugin {

QString LoaderPlugin::AskForFileName(const QString& fileNameFilter)
{
    QSettings settings(QLatin1String{"HDPS"}, QLatin1String{"Plugins/"} + getKind());
    const QLatin1String directoryPathKey("directoryPath");
    const auto directoryPath = settings.value(directoryPathKey).toString();
    const auto fileName = QFileDialog::getOpenFileName(
        nullptr, QObject::tr("Load File"), directoryPath, fileNameFilter + QObject::tr(";;All Files (*)"));

    // Only store the directory name when the dialog has not been not canceled and the file name is non-empty.
    if (!fileName.isEmpty())
    {
        settings.setValue(directoryPathKey, QFileInfo(fileName).absolutePath());
    }
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
}
