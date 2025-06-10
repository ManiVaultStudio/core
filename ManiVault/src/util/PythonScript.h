// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "Script.h"

namespace mv::util
{

/**
 * Python script class
 *
 * For running a Python-based script
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT PythonScript : public Script
{
    Q_OBJECT

public:

    /**
     * Construct script with \p type and \p language
     * @param title Script title
     * @param type Script type
     * @param location Script location
     * @param datasets List of datasets that the script can work with (optional, default is empty)
     * @param languageVersion Version of the scripting language
     * @param parent Pointer to parent object (optional, default is nullptr)
     */
    explicit PythonScript(const QString& title, const Type& type, const QString& location, const Datasets& datasets, const Version& languageVersion = Version(3, 12, 0), QObject* parent = nullptr);

    /** Runs the script */
    void run() override;
};

using PythonScripts = std::vector<const PythonScript*>;

}
