// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2026 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include <QString>

namespace mv
{
/**
 * @brief Stable project-relative identifier for a resource stored in a project archive.
 *
 * Plugins may serialize this identifier alongside metadata in their own format.
 * It contains no temporary filesystem path and does not describe the resource's
 * encoding or internal layout.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ProjectResourceReference
{
public:
    /** Creates an invalid reference. */
    ProjectResourceReference() = default;

    /** Creates a reference from a serialized project-resource URI. */
    explicit ProjectResourceReference(QString uri);

    /** Returns the serialized URI, or an empty string when this reference is invalid. */
    QString toString() const;

    /** Returns whether this reference has the project-resource scheme and a resource path. */
    bool isValid() const;

private:
    QString _uri; /**< Project-relative resource URI persisted by a plugin serializer. */
};
}
