// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowResultRegistry.h"
#include <QMutexLocker>

namespace mv::workflow
{
WorkflowResultRegistry& WorkflowResultRegistry::instance()
{
	static WorkflowResultRegistry registry;

	return registry;
}

QUuid WorkflowResultRegistry::add(SharedWorkflowResult result)
{
    QMutexLocker lock(&_mutex);

	const auto id = QUuid::createUuid();

	_results.insert(id, std::move(result));

	return id;
}

SharedWorkflowResult WorkflowResultRegistry::get(const QUuid& id) const
{
    QMutexLocker lock(&_mutex);

	return _results.value(id);
}

void WorkflowResultRegistry::remove(const QUuid& id)
{
    QMutexLocker lock(&_mutex);

	_results.remove(id);
}

}
