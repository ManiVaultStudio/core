// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2026 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "StagedProjectResourceFile.h"

namespace mv
{
StagedProjectResourceFile::StagedProjectResourceFile(QString filePath, const std::uint64_t size, std::shared_ptr<const void> lifetime) :
    _filePath(std::move(filePath)),
    _size(size),
    _lifetime(std::move(lifetime))
{
}

QString StagedProjectResourceFile::filePath() const
{
    return _filePath;
}

std::uint64_t StagedProjectResourceFile::size() const
{
    return _size;
}

bool StagedProjectResourceFile::isValid() const
{
    return !_filePath.isEmpty() && static_cast<bool>(_lifetime);
}
}
