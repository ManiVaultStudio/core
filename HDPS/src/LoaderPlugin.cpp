#include "LoaderPlugin.h"

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

}
}
