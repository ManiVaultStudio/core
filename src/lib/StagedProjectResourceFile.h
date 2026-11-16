// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2026 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include <QString>

#include <cstdint>
#include <memory>
#include <utility>

namespace mv
{
/**
 * @brief Lease keeping a staged project resource available as a local file.
 *
 * A lease owns the temporary staging area that contains the file. The path is
 * valid while this lease or another lease for that file remains alive.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT StagedProjectResourceFile
{
public:
    /** Creates an empty lease. */
    StagedProjectResourceFile() = default;

    /** Creates a lease for a staged file and its lifetime owner. */
    StagedProjectResourceFile(QString filePath, std::uint64_t size, std::shared_ptr<const void> lifetime);

    /** Returns the staged file path, or an empty path when the lease is invalid. */
    QString filePath() const;

    /** Returns the uncompressed resource size in bytes. */
    std::uint64_t size() const;

    /** Returns whether this lease keeps a staged file alive. */
    bool isValid() const;

private:
    QString                     _filePath; /**< Local path usable by seek-based readers while the lease is alive. */
    std::uint64_t               _size = 0; /**< Uncompressed resource size in bytes. */
    std::shared_ptr<const void> _lifetime; /**< Owner that keeps the temporary directory and file alive. */
};
}
