// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#ifndef HDPS_POINTVIEW_H
#define HDPS_POINTVIEW_H

#include <cstddef> // For size_t
#include <cassert>

namespace mv
{
    /* Allows iterating over the values of a single point from a point data buffer.
    Supports using a range-based for-loop, like `for (auto& value: pointView)`.
    */
    template <typename DataIteratorType>
    class PointView
    {
    public:
        using iterator = DataIteratorType;

        PointView() = default;

        PointView(
            const iterator begin,
            const iterator end,
            const unsigned index)
            :
            _begin{ begin },
            _end{ end },
            _index{ index }
        {
            assert(begin <= end);
        }

        iterator begin() const
        {
            return _begin;
        }

        iterator end() const
        {
            return _end;
        }

        auto& operator[](std::size_t i) const
        {
            return _begin[i];
        }

        std::size_t size() const
        {
            return _end - _begin;
        }

        bool empty() const
        {
            return _begin == _end;
        }

        auto index() const
        {
            return _index;
        }

        friend bool operator==(const PointView& lhs, const PointView& rhs)
        {
            return (lhs._begin == rhs._begin) && (lhs._end == rhs._end);
        }

        friend bool operator!=(const PointView& lhs, const PointView& rhs)
        {
            return ! (lhs == rhs);
        }

    private:
        iterator _begin{};
        iterator _end{};
        unsigned _index{};
    };
}


#endif // HDPS_POINTVIEW_H
