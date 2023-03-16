#pragma once

#include <QPair>

namespace hdps::util {

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
    NumericalRange(NumericalType minimum, NumericalType maximum) :
        QPair<NumericalType, NumericalType>(std::numeric_limits<NumericalType>::lowest(), std::numeric_limits<NumericalType>::max())
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
        this->first = std::min(minimum, this->second);
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
        this->second = std::max(maximum, this->first);
    }
};

}
