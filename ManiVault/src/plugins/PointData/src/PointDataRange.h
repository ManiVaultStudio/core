// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "PointDataIterator.h"
#include "RandomAccessRange.h"

#include <iterator> // For begin and end.

namespace mv
{
    /* A PointDataRange is a random access range between two iterators of type PointDataIterator.
    */
    template <typename ValueIteratorType, typename IndexIteratorType, typename IndexFunctionType>
    using PointDataRange = RandomAccessRange<PointDataIterator<ValueIteratorType, IndexIteratorType, IndexFunctionType>>;


    /* Makes a PointDataRange object for a subset of the values specified by the
    * first parameter. The subset is specified by the indices provided by the
    * second parameter.
    */
    template <typename ValueIteratorType, typename IndexContainerType, typename IndexFunctionType>
    auto makePointDataRangeOfSubset(
        const ValueIteratorType beginOfValueContainer,
        const IndexContainerType& indices,
        const unsigned numberOfDimensions,
        const IndexFunctionType indexFunction)
    {
        using IndexIteratorType = decltype(begin(indices));
        using PointDataIteratorType = PointDataIterator<ValueIteratorType, IndexIteratorType, IndexFunctionType>;

        return PointDataRange<ValueIteratorType, IndexIteratorType, IndexFunctionType>
        {
            PointDataIteratorType(beginOfValueContainer, begin(indices), numberOfDimensions, indexFunction),
                PointDataIteratorType(beginOfValueContainer, end(indices), numberOfDimensions, indexFunction)
        };
    }


    /* Makes a PointDataRange object for a full set of values.
    */
    template <typename ValueIteratorType, typename IndexFunctionType>
    auto makePointDataRangeOfFullSet(
        const ValueIteratorType beginOfValueContainer,
        const ValueIteratorType endOfValueContainer,
        const unsigned numberOfDimensions,
        const IndexFunctionType indexFunction)
    {
        using PointDataIteratorType = PointDataIterator<ValueIteratorType, unsigned, IndexFunctionType>;
        const auto numberOfPoints = static_cast<unsigned>( (endOfValueContainer - beginOfValueContainer) / numberOfDimensions);

        return PointDataRange<ValueIteratorType, unsigned, IndexFunctionType>
        {
            PointDataIteratorType(beginOfValueContainer, 0U, numberOfDimensions, indexFunction),
                PointDataIteratorType(beginOfValueContainer, numberOfPoints, numberOfDimensions, indexFunction)
        };
    }


}
