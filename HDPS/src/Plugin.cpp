#include "Plugin.h"

#include "Application.h"

namespace hdps
{

namespace plugin
{

QVariant Plugin::getSetting(const QString& path, const QVariant& defaultValue /*= QVariant()*/) const
{
    return Application::current()->getSetting(QString("%1/%2").arg(_kind, path), defaultValue);
}

void Plugin::setSetting(const QString& path, const QVariant& value)
{
    Application::current()->setSetting(QString("%1/%2").arg(_kind, path), value);
}

}
}