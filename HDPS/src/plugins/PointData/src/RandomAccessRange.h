// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#ifndef HDPS_RANDOMACCESSRANGE_H
#define HDPS_RANDOMACCESSRANGE_H

#include <cstddef> // For size_t
#include <cassert>

namespace hdps
{
    template <typename RandomAccessIteratorType>
    class RandomAccessRange
    {
    public:
        using iterator = RandomAccessIteratorType;

        RandomAccessRange() = default;

        RandomAccessRange(const iterator begin, const iterator end)
            :
            _begin{ begin },
            _end{ end }
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

        decltype(auto) operator[](std::size_t i) const
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

        friend bool operator==(const RandomAccessRange& lhs, const RandomAccessRange& rhs)
        {
            return (lhs._begin == rhs._begin) && (lhs._end == rhs._end);
        }

        friend bool operator!=(const RandomAccessRange& lhs, const RandomAccessRange& rhs)
        {
            return ! (lhs == rhs);
        }

    private:
        iterator _begin{};
        iterator _end{};
    };

}


#endif // HDPS_RANDOMACCESSRANGE_H
