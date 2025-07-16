// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "util/Miscellaneous.h"

#include "StyledIcon.h"

#include <QStandardItem>
#include <QString>

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

	/**
     * Construct with hardware component specification \p title
	 * @param title hardware component specification title
	 */
	HardwareComponentSpec(const QString& title);

    virtual ~HardwareComponentSpec() = default;

    /**
     * Get the title of the hardware component specification
     * @return String containing the title of the hardware component specification
     */
    QString getTitle() const { return _title; }

	/**
     * Load the hardware component spec from \p variantMap
     * @param variantMap Variant map containing the hardware component spec properties
     */
    virtual void fromVariantMap(const QVariantMap& variantMap);

	/**
     * Convert the hardware component spec to a variant map
     * @return Variant map containing the hardware component spec properties
	 */
	virtual QVariantMap toVariantMap() const;

    /**
     * Get the reason why the hardware component spec does not meet the \p required hardware component spec
     * @param required Hardware component spec that is required
     * @return String containing the reason why the hardware component spec does not meet the required hardware component spec
     */
    virtual QString getFailureString(const HardwareComponentSpec& required) const = 0;

    /**
     * Get whether the hardware component spec has been initialized
     * @return Boolean determining whether the hardware component spec has been initialized
     */
    bool isInitialized() const { return _initialized; }

    /**
     * Get whether the hardware component specification is smaller than the \p other hardware component specification
     * @param other Hardware component specification to compare with
     * @return Boolean determining whether the hardware component specification is smaller than the \p other hardware component specification
     */
    virtual bool lessThan(const HardwareComponentSpec& other) const = 0;

    /**
     * Get whether the hardware component specification is equal to the \p other hardware component specification
     * @param other Hardware component specification to compare with
     * @return Boolean determining whether the hardware component specification is equal to the \p other hardware component specification
     */
    virtual bool equals(const HardwareComponentSpec& other) const = 0;

    /**
     * Get whether the hardware component specification meets the \p required hardware component specification
     * @param required Hardware component specification that is required
     * @return Boolean determining whether the hardware component spec meets the required hardware component spec
     */
    virtual bool meets(const HardwareComponentSpec& required) const = 0;

    /**
     * Get standard item
     * @return Pointer to standard item representing the hardware component spec
     */
    virtual QStandardItem* getStandardItem() const;

    /**
     * Get row representing the hardware component spec parameter
     * @param parameterName Name of the parameter
     * @param systemValue String containing the system value of the parameter
     * @param requiredValue String containing the required value of the parameter
     * @param valid Boolean determining whether the parameter condition is valid
     * @return List of pointers to standard items representing the parameter
     */
    static QList<QStandardItem*> getParameterRow(const QString& parameterName, const QString& systemValue = "", const QString& requiredValue = "", bool valid = true);

protected: // Population methods

    /** Load the hardware component spec from current system */
    virtual void fromSystem() = 0;

	/**
     * Set whether the hardware spec has been initialized to \p initialized
     * @param initialized Boolean determining whether the hardware spec has been initialized
	 */
	void setInitialized(bool initialized = true) { _initialized = initialized; }

private:
    const QString   _title;         /** Hardware component specification title */
    bool            _initialized;   /** Whether the hardware spec has been initialized (either from variant map or system) */

    friend class HardwareSpec; // Allow HardwareSpec to access private members
};

/** Shared pointer to hardware component specification */
using HardwareComponentSpecPtr = std::shared_ptr<HardwareComponentSpec>;

/** Shared pointers to hardware component specification */
using HardwareComponentSpecs = std::vector<std::shared_ptr<HardwareComponentSpec>>;     

}