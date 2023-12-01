// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QTemporaryDir>

namespace mv::util {

/**
 * Temporary directory class
 *
 * Wrapper around QTemporaryDir to ensure that all ManiVault temporary data will reside somewhere in /temp/ManiVault
 *
 * @author Thomas Kroes
 */
class TemporaryDir final : public QTemporaryDir
{
public:

    /**
     * Construct temporary dir
     * Constructs the QTemporaryDir with a temp/ManiVault template path
     */
    TemporaryDir();

    /** Do not allow users of the temporary dir to construct a temporary dir with their own template path */
    TemporaryDir(const QString& templatePath) = delete;
};

}