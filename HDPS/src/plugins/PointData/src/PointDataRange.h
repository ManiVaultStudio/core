#ifndef HDPS_POINTDATARANGE_H
#define HDPS_POINTDATARANGE_H

#include "PointDataIterator.h"
#include "RandomAccessRange.h"

#include <iterator> // For begin and end.

namespace hdps
{
    /* A PointDataRange is a random access range between two iterators of type PointDataIterator.
    */
    template <typename ValueIteratorType, typename IndexIteratorType>
    using PointDataRange = RandomAccessRange<PointDataIterator<ValueIteratorType, IndexIteratorType>>;


    /* Makes a PointDataRange object for a subset of the values specified by the
    * first parameter. The subset is specified by the indices provided by the
    * second parameter.
    */
    template <typename ValueIteratorType, typename IndexContainerType>
    auto makePointDataRange(
        const ValueIteratorType beginOfValueContainer,
        const IndexContainerType& indices,
        const unsigned numberOfDimensions)
    {
        using IndexIteratorType = decltype(begin(indices));
        using PointDataIteratorType = PointDataIterator<ValueIteratorType, IndexIteratorType>;

        return PointDataRange<ValueIteratorType, IndexIteratorType>
        {
            PointDataIteratorType(beginOfValueContainer, begin(indices), numberOfDimensions),
                PointDataIteratorType(beginOfValueContainer, end(indices), numberOfDimensions)
        };
    }


    /* Makes a PointDataRange object for a full set of values.
    */
    template <typename ValueIteratorType>
    auto makePointDataRange(
        const ValueIteratorType beginOfValueContainer,
        const ValueIteratorType endOfValueContainer,
        const unsigned numberOfDimensions)
    {
        using PointDataIteratorType = PointDataIterator<ValueIteratorType, unsigned>;
        const auto numberOfPoints = static_cast<unsigned>( (endOfValueContainer - beginOfValueContainer) / numberOfDimensions);

        return PointDataRange<ValueIteratorType, unsigned>
        {
            PointDataIteratorType(beginOfValueContainer, 0U, numberOfDimensions),
                PointDataIteratorType(beginOfValueContainer, numberOfPoints, numberOfDimensions)
        };
    }


}


#endif // HDPS_POINTDATARANGE_H
