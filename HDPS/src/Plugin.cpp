#include "Plugin.h"

#include "Application.h"

namespace hdps
{

namespace plugin
{

QVariant Plugin::getSetting(const QString& path, const QVariant& defaultValue /*= QVariant()*/) const
{
    return Application::getPluginSetting(_kind, path, defaultValue);
}

void Plugin::setSetting(const QString& path, const QVariant& value)
{
    Application::setPluginSetting(_kind, path, value);
}

}
}