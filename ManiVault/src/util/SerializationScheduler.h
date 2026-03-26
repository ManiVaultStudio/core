// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include "DecodeRequestState.h"

#include <util/Serialization.h>

#include <QObject>

CORE_EXPORT class SerializationScheduler : public QObject
{
    Q_OBJECT

public:
    static SerializationScheduler& instance();

    DecodeRequestHandle submitDecode(const QVariantMap& variantMap, char* destination, mv::util::ConcurrencyMode concurrencyMode = mv::util::ConcurrencyMode::Parallel);

private:
    explicit SerializationScheduler(QObject* parent = nullptr);
    SerializationScheduler(const SerializationScheduler&) = delete;
    SerializationScheduler& operator=(const SerializationScheduler&) = delete;

    QThreadPool     _pool;
};
