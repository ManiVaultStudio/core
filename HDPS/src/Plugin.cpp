#include "Plugin.h"

hdps::plugin::Plugin::Plugin(Type type, QString kind) :
    _name(kind + QUuid::createUuid().toString()),
    _guiName(QString("%1 %2").arg(kind, QString::number(_noInstances[kind] + 1))),
    _kind(kind),
    _type(type)
{
    _noInstances[kind]++;
}

QMap<QString, std::int32_t> hdps::plugin::Plugin::_noInstances = QMap<QString, std::int32_t>();