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
class CORE_EXPORT HardwareComponentSpec : public Serializable
{
public:

	/**
     * Comnstruct with hardware component specification \p title
	 * @param title hardware component specification title
	 */
	HardwareComponentSpec(const QString& title);

    virtual ~HardwareComponentSpec() = default;

    /**
     * Get whether the hardware component spec has been initialized
     * @return Boolean determining whether the hardware component spec has been initialized
     */
    bool isInitialized() const { return _initialized; }

protected: // Population methods

    /** Load the hardware spec from current system */
    virtual void fromSystem() = 0;

	/**
     * Set whether the hardware spec has been initialized to \p initialized
     * @param initialized Boolean determining whether the hardware spec has been initialized
	 */
	void setInitialized(bool initialized = true) { _initialized = initialized; }

private:
    bool    _initialized;   /** Whether the hardware spec has been initialized (either from variant map or system) */

    friend class HardwareSpec; // Allow HardwareSpec to access private members
};

}