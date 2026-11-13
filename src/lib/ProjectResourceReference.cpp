// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2026 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "ProjectResourceReference.h"

#include <utility>

namespace mv
{
ProjectResourceReference::ProjectResourceReference(QString uri) :
    _uri(std::move(uri))
{
}

QString ProjectResourceReference::toString() const
{
    return _uri;
}

bool ProjectResourceReference::isValid() const
{
    return _uri.startsWith(QStringLiteral("project-resource://resources/"));
}
}
