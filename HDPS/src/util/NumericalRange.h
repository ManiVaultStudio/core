// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QPair>
#include <algorithm>

namespace mv::util {

/**
 * Numerical range class
 *
 * Stores a numerical range
 *
 * @author Thomas Kroes
 */
template<typename NumericalType>
class NumericalRange : public QPair<NumericalType, NumericalType>
{
public:

    /**
     * Construct from /p minimum and \p maximum
     * @param minimum Range minimum
     * @param maximum Range maximum
     */
    NumericalRange(NumericalType minimum = std::numeric_limits<NumericalType>::max(), NumericalType maximum = std::numeric_limits<NumericalType>::lowest()) :
        QPair<NumericalType, NumericalType>(std::numeric_limits<NumericalType>::max(), std::numeric_limits<NumericalType>::lowest())
    {
        setMinimum(minimum);
        setMaximum(maximum);
    }

    /** Get range minimum */
    NumericalType getMinimum() const {
        return this->first;
    }

    /**
     * Set range minimum to \p minimum
     * @param minimum Range minimum
     */
    void setMinimum(NumericalType minimum) {
        this->first = std::min(minimum, this->first);
    }

    /** Get range maximum */
    NumericalType getMaximum() const {
        return this->second;
    }

    /**
     * Set range maximum to \p maximum
     * @param maximum Range maximum
     */
    void setMaximum(NumericalType maximum) {
        this->second = std::max(maximum, this->second);
    }

    /**
     * Addition operator
     * @return Added range
     */
    NumericalRange<NumericalType>& operator += (const NumericalRange<NumericalType>& other)
    {
        this->first     = std::min(this->first, other.getMinimum());
        this->second    = std::max(this->second, other.getMaximum());

        return *this;
    }

    /**
     * Equality operator
     * @param rhs Right-hand-side operator
     */
    const bool operator == (const NumericalRange<NumericalType>& rhs) const {
        return rhs.getMinimum() == getMinimum() && rhs.getMaximum() == getMaximum();
    }

    /**
     * Inequality operator
     * @param rhs Right-hand-side operator
     */
    const bool operator != (const NumericalRange<NumericalType>& rhs) const {
        return rhs.getMinimum() != getMinimum() || rhs.getMaximum() != getMaximum();
    }
};

}
