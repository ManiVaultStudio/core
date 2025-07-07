// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

namespace mv::util
{

/**
 * Hardware component specification class
 * Base class for encapsulating a component specification
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT HardwareComponentSpec
{
public:

    using HardwareComponentSpecPtr = std::shared_ptr<HardwareComponentSpec>; /**< Pointer to hardware component specification */

	/**
     * Comnstruct with hardware component specification \p title
	 * @param title hardware component specification title
	 */
	HardwareComponentSpec(const QString& title);

    virtual ~HardwareComponentSpec() = default;

    /**
     * Load the hardware component spec from \p variantMap
     * @param variantMap Variant map containing the hardware component spec properties
     */
    virtual void fromVariantMap(const QVariantMap& variantMap);

	/**
     * Compare with \p other hardware component spec and produce a status string (empty if no ok)
     * @param other Hardware component spec to compare with
     * @return Status string indicating the status of the hardware component spec compared to \p other
	 */
	virtual QString getStatusString(const HardwareComponentSpecPtr& other) const = 0;

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
    const QString&  _title;         /** Hardware component specification title */
    bool            _initialized;   /** Whether the hardware spec has been initialized (either from variant map or system) */

    friend class HardwareSpec; // Allow HardwareSpec to access private members
};

using HardwareComponentSpecs = std::vector<std::shared_ptr<HardwareComponentSpec>>;

}