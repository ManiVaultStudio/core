#ifndef HDPS_POINTDATAITERATOR_H
#define HDPS_POINTDATAITERATOR_H

#include <iterator>

#include "PointView.h"

namespace hdps
{
    template <typename ValueIteratorType, typename IndexIteratorType>
    class PointDataIterator
    {
        /* For an unsigned index argument, the value of the index is the value of the argument.
        */
        static auto valueOfIndex(const unsigned index)
        {
            return index;
        }

        /* For an iterator, the value of the index comes from dereferencing the iterator.
        */
        template <typename T>
        static auto valueOfIndex(T indexIterator)
        {
            return *indexIterator;
        }


        // Its data members: 
        ValueIteratorType _valueIterator{};
        IndexIteratorType _indexIterator{};
        unsigned _numberOfDimensions{};

        using PointViewType = PointView<ValueIteratorType>;
    public:
        // Types conforming the iterator requirements of the C++ standard library:
        using difference_type = std::ptrdiff_t;
        using value_type = PointViewType;
        using reference = PointViewType;
        using pointer = const PointViewType*;
        using iterator_category = std::random_access_iterator_tag;


        /* Explicitly defaulted default-constructor
        */
        PointDataIterator() = default;


        PointDataIterator(
            const ValueIteratorType valueIterator,
            const IndexIteratorType indexIterator,
            const unsigned numberOfDimensions)
            :
            _valueIterator{ valueIterator },
            _indexIterator{ indexIterator },
            _numberOfDimensions{ numberOfDimensions }
        {
        }


        /**  Returns a PointView object to the current point.
        */
        auto operator*() const
        {
            const auto index = valueOfIndex(_indexIterator);
            const auto begin = _valueIterator + (index * _numberOfDimensions);
            const auto end = begin + _numberOfDimensions;
            return PointViewType(begin, end, index);
        }


        /** Prefix increment ('++it').
        */
        auto& operator++()
        {
            ++(_indexIterator);
            return *this;
        }


        /** Postfix increment ('it++').
         * \note Usually prefix increment ('++it') is preferable.
         */
        auto operator++(int)
        {
            auto result = *this;
            ++(*this);
            return result;
        }


        /** Prefix decrement ('--it').
        */
        auto& operator--()
        {
            --_indexIterator;
            return *this;
        }


        /** Postfix increment ('it--').
         * \note  Usually prefix increment ('--it') is preferable.
         */
        auto operator--(int)
        {
            auto result = *this;
            --(*this);
            return result;
        }



        /** Does (it += n) for iterator 'it' and integer value 'n'.
        */
        friend auto& operator+=(PointDataIterator& it, const difference_type n)
        {
            it._indexIterator += n;
            return it;
        }


        /** Returns (it1 - it2) for iterators it1 and it2.
        */
        friend difference_type operator-(const PointDataIterator& it1, const PointDataIterator& it2)
        {
            return it1._indexIterator - it2._indexIterator;
        }


        /** Returns (it + n) for iterator 'it' and integer value 'n'.
        */
        friend auto operator+(PointDataIterator it, const difference_type n)
        {
            return it += n;
        }


        /** Returns (n + it) for iterator 'it' and integer value 'n'.
        */
        friend auto operator+(const difference_type n, PointDataIterator it)
        {
            return it += n;
        }


        /** Returns (it - n) for iterator 'it' and integer value 'n'.
        */
        friend auto operator-(PointDataIterator it, const difference_type n)
        {
            return it += (-n);
        }


        /** Returns it[n] for iterator 'it' and integer value 'n'.
        */
        auto operator[](const difference_type n) const
        {
            return *(*this + n);
        }


        /** Returns (it1 == it2) for iterators it1 and it2.
        */
        friend bool operator==(const PointDataIterator& it1, const PointDataIterator& it2)
        {
            return it1._indexIterator == it2._indexIterator;
        }


        /** Returns (it1 != it2) for iterators it1 and it2.
        */
        friend bool operator!=(const PointDataIterator& it1, const PointDataIterator& it2)
        {
            return ! (it1 == it2);
        }


        /** Returns (it1 < it2) for iterators it1 and it2.
        */
        friend bool operator<(const PointDataIterator& it1, const PointDataIterator& it2)
        {
            return it1._indexIterator < it2._indexIterator;
        }


        /** Returns (it1 > it2) for iterators it1 and it2.
        */
        friend bool operator>(const PointDataIterator& it1, const PointDataIterator& it2)
        {
            // Implemented just like the corresponding std::rel_ops operator.
            return it2 < it2;
        }


        /** Returns (it1 <= it2) for iterators it1 and it2.
        */
        friend bool operator<=(const PointDataIterator& it1, const PointDataIterator& it2)
        {
            // Implemented just like the corresponding std::rel_ops operator.
            return !(it2 < it1);
        }


        /** Returns (it1 >= it2) for iterators it1 and it2.
        */
        friend bool operator>=(const PointDataIterator& it1, const PointDataIterator& it2)
        {
            // Implemented just like the corresponding std::rel_ops operator.
            return !(it1 < it2);
        }


        /** Does (it -= n) for iterator 'it' and integer value 'n'.
        */
        friend auto& operator-=(PointDataIterator& it, const difference_type n)
        {
            it += (-n);
            return it;
        }


        /** Returns the index of the current point.
        */
        friend auto index(const PointDataIterator& arg)
        {
            return valueOfIndex(arg._indexIterator);
        }

    };

}


#endif // HDPS_POINTDATAITERATOR_H
