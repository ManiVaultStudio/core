// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2026 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "StagedProjectResourceFile.h"

#include <QByteArray>

#include <cstdint>

namespace mv
{
/**
 * @brief Read handle for one project resource.
 *
 * The handle is bound to the project archive resolved when it is opened. It
 * reads bounded byte ranges and can provide a staged-file lease for consumers
 * that need filesystem access. A handle remains associated with its original
 * project if the project manager later switches projects.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ProjectResourceHandle
{
public:
    virtual ~ProjectResourceHandle();

    /** Returns the uncompressed resource size in bytes. */
    virtual std::uint64_t size() const = 0;

    /** Reads a bounded byte range; throws std::out_of_range when it exceeds the resource. */
    virtual QByteArray readRange(std::uint64_t offset, std::uint64_t size) const = 0;

    /** Stages the resource and returns a lease for its local file. */
    virtual StagedProjectResourceFile stagedFile() const = 0;
};
}
