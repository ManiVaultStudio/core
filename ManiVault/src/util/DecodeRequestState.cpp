// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "DecodeRequestState.h"

#ifdef _DEBUG
	#define DECODE_REQUEST_STATE_VERBOSE
#endif

DecodeRequestState::DecodeRequestState(QObject* parent) :
    QObject(parent)
{
}

bool DecodeRequestState::isFinished() const
{
    QMutexLocker lock(&_mutex);

    return _finished;
}

bool DecodeRequestState::hasError() const
{
    QMutexLocker lock(&_mutex);

    return !_error.isEmpty();
}

QString DecodeRequestState::errorString() const
{
    QMutexLocker lock(&_mutex);

    return _error;
}

int DecodeRequestState::totalJobs() const
{
    QMutexLocker lock(&_mutex);

    return _totalJobs;
}

int DecodeRequestState::completedJobs() const
{
    QMutexLocker lock(&_mutex);

    return _completedJobs;
}