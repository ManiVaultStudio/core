// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "VariantMapWorkflowContext.h"

namespace mv::util
{

QVariantMap VariantMapWorkflowContext::getMap() const
{
	QMutexLocker locker(&_mutex);
	return _map;
}

void VariantMapWorkflowContext::setMap(const QVariantMap& map)
{
	QMutexLocker locker(&_mutex);
	_map = map;
}

bool VariantMapWorkflowContext::contains(const QString& key) const
{
	QMutexLocker locker(&_mutex);
	return _map.contains(key);
}

QVariant VariantMapWorkflowContext::value(const QString& key, const QVariant& defaultValue) const
{
	QMutexLocker locker(&_mutex);
	return _map.value(key, defaultValue);
}

QVariantMap VariantMapWorkflowContext::mapValue(const QString& key) const
{
	QMutexLocker locker(&_mutex);
	return _map.value(key).toMap();
}

void VariantMapWorkflowContext::setValue(const QString& key, const QVariant& value)
{
	QMutexLocker locker(&_mutex);
    _map.insert(key, value);
}

QVariant VariantMapWorkflowContext::takeValue(const QString& key)
{
	QMutexLocker locker(&_mutex);
	return _map.take(key);
}

void VariantMapWorkflowContext::merge(const QVariantMap& values)
{
    QMutexLocker locker(&_mutex);

    for (auto it = values.constBegin(); it != values.constEnd(); ++it)
        _map.insert(it.key(), it.value());
}

void VariantMapWorkflowContext::clear()
{
	QMutexLocker locker(&_mutex);
    _map.clear();
}

}