// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowContextVariantMap.h"

namespace mv::workflow
{

QVariantMap WorkflowContextVariantMap::getMap() const
{
	QMutexLocker locker(&_mutex);
	return _map;
}

void WorkflowContextVariantMap::setMap(const QVariantMap& map)
{
	QMutexLocker locker(&_mutex);
	_map = map;
}

bool WorkflowContextVariantMap::contains(const QString& key) const
{
	QMutexLocker locker(&_mutex);
	return _map.contains(key);
}

QVariant WorkflowContextVariantMap::value(const QString& key, const QVariant& defaultValue) const
{
	QMutexLocker locker(&_mutex);
	return _map.value(key, defaultValue);
}

QVariantMap WorkflowContextVariantMap::mapValue(const QString& key) const
{
	QMutexLocker locker(&_mutex);
	return _map.value(key).toMap();
}

void WorkflowContextVariantMap::setValue(const QString& key, const QVariant& value)
{
	QMutexLocker locker(&_mutex);
    _map.insert(key, value);
}

QVariant WorkflowContextVariantMap::takeValue(const QString& key)
{
	QMutexLocker locker(&_mutex);
	return _map.take(key);
}

void WorkflowContextVariantMap::merge(const QVariantMap& values)
{
    QMutexLocker locker(&_mutex);

    for (auto it = values.constBegin(); it != values.constEnd(); ++it)
        _map.insert(it.key(), it.value());
}

void WorkflowContextVariantMap::clear()
{
	QMutexLocker locker(&_mutex);
    _map.clear();
}

}