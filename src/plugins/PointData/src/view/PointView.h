// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <cstddef> // For size_t
#include <cassert>

namespace mv
{
    /**
     * @brief Non-owning iterator range for one point's dimension values.
     *
     * The view stores the iterator bounds and point index but does not own the
     * underlying point-data buffer. Its iterators and indexed access remain
     * valid only while that buffer remains alive and unmoved.
     *
     * @author Thomas Kroes
     */
    template <typename DataIteratorType>
    class PointView
    {
    public:
        using iterator = DataIteratorType;

        /** Creates an empty view with default-constructed iterators. */
        PointView() = default;

        /** Creates a view over [begin, end) and records the point's logical index. */
        PointView(
            const iterator begin,
            const iterator end,
            const std::uint64_t index)
            :
            _begin{ begin },
            _end{ end },
            _index{ index }
        {
            assert(begin <= end);
        }

        /** Returns the first dimension iterator. */
        iterator begin() const
        {
            return _begin;
        }

        /** Returns the one-past-last dimension iterator. */
        iterator end() const
        {
            return _end;
        }

        /** Returns the dimension value at the supplied zero-based offset. */
        auto& operator[](std::size_t i) const
        {
            return _begin[i];
        }

        /** Returns the number of dimensions in the view. */
        std::size_t size() const
        {
            return _end - _begin;
        }

        /** Returns whether the view has no dimension values. */
        bool empty() const
        {
            return _begin == _end;
        }

        /** Returns the logical point index associated with this view. */
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
        iterator _begin{};      /**< First iterator into the non-owned point buffer. */
        iterator _end{};        /**< One-past-last iterator into the non-owned point buffer. */
        std::uint64_t _index{}; /**< Logical index of the point represented by this view. */
    };
}
