#include "Workspace.h"

#include <Application.h>

using namespace hdps;

Workspace::Workspace(QObject* parent /*= nullptr*/) :
    QObject(parent)
{
}

void Workspace::fromVariantMap(const QVariantMap& variantMap)
{
}

QVariantMap Workspace::toVariantMap() const
{
    return QVariantMap();
}

QString Workspace::getFilePath() const
{
    return _filePath;
}

void Workspace::setFilePath(const QString& filePath)
{
    _filePath = filePath;

    emit filePathChanged(_filePath);
}
