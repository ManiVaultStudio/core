#pragma once

#include "DatasetRef.h"

namespace hdps
{

namespace util
{

DatasetRefPrivate::DatasetRefPrivate(QObject* parent /*= nullptr*/) :
    QObject(parent),
    EventListener()
{
    setEventCore(Application::core());
}

}
}
