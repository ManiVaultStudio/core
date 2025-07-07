// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "Serializable.h"

namespace mv::util
{

/**
 * Hardware component specification class
 * Base class for encapsulating a component specification
 *
 * @author Thomas Kroes
 */
CORE_EXPORT class HardwareComponentSpec : public Serializable
{
public:
    virtual ~HardwareComponentSpec() = default;

protected: // Population methods

    /** Load the hardware spec from current system */
    virtual void fromSystem() = 0;

    friend class HardwareSpec; // Allow HardwareSpec to access private members
};

}