#include "LoaderPlugin.h"

#include <actions/PluginTriggerAction.h>

#include <QFileDialog>
#include <QSettings>
#include <QString>

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
    const auto pluginTriggerAction = createPluginTriggerAction(QString("%1").arg(getKind()), QString("Load %1").arg(getKind()), Datasets());

    connect(pluginTriggerAction, &QAction::triggered, this, [this]() -> void {
        Application::core()->requestPlugin(getKind());
    });

    return { pluginTriggerAction };
}

PluginTriggerActions LoaderPluginFactory::getPluginTriggerActions(const DataTypes& dataTypes) const
{
    return getPluginTriggerActions(DataTypes({}));
}

}
}
