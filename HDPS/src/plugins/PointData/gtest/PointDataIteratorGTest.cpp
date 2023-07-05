// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

// The file to be tested:
#include <PointDataRange.h>

// GoogleTest header file:
#include <gtest/gtest.h>

#include <vector>

// Test template instantiations for the most common template arguments:
template class hdps::PointDataIterator<float*, const unsigned*, unsigned(*)(const unsigned*)>;
template class hdps::PointDataIterator<std::vector<float>::iterator, const unsigned*, unsigned(*)(const unsigned*)>;
template class hdps::PointDataIterator<std::vector<float>::iterator, std::vector<unsigned>::const_iterator, unsigned(*)(std::vector<unsigned>::const_iterator)>;
template class hdps::PointDataIterator<std::vector<float>::const_iterator, const unsigned*, unsigned(*)(const unsigned*)>;
template class hdps::PointDataIterator<std::vector<float>::const_iterator, std::vector<unsigned>::const_iterator, unsigned(*)(std::vector<unsigned>::const_iterator)>;

using hdps::PointDataIterator;


TEST(PointDataIterator, satisfiesRandomAccessIteratorRequirements)
{
    std::vector<float> inputData{ 0.0f, 1.1f, 2.2f, 3.3f, 4.4f, 5.5f };
    const std::vector<unsigned> indices{ 1U, 2U, 3U, 4U };
    constexpr auto numberOfDimensions = 1;

    // Unit test borrowed from TEST(ImageBufferRange, IteratorsSupportRandomAccess), from
    // https://github.com/InsightSoftwareConsortium/ITK/blob/v5.1.0/Modules/Core/Common/test/itkImageBufferRangeGTest.cxx#L648

    // Testing expressions from Table 111 "Random access iterator requirements
    // (in addition to bidirectional iterator)", C++11 Standard, section 24.2.7
    // "Random access iterator" [random.access.iterators].

    // Note: The 1-letter identifiers (X, a, b, n, r) and the operational semantics
    // are directly from the C++11 Standard.
    const auto indexFunction = [](const unsigned* ptr) {return *ptr; };
    using X = PointDataIterator<float*, const unsigned*, unsigned(*)(const unsigned*)>;
    X a(inputData.data(), indices.data() + indices.size(), numberOfDimensions, indexFunction);
    X b(inputData.data(), indices.data(), numberOfDimensions, indexFunction);

    const X initialIterator(inputData.data(), indices.data(), numberOfDimensions, indexFunction);
    X       mutableIterator = initialIterator;
    X& r = mutableIterator;

    using difference_type = std::iterator_traits<X>::difference_type;
    using reference = std::iterator_traits<X>::reference;

    {
        // Expression to be tested: 'r += n'
        difference_type n = 3;

        r = initialIterator;
        const auto expectedResult = [&r, n] {
            // Operational semantics, as specified by the C++11 Standard:
            difference_type m = n;
            if (m >= 0)
                while (m--)
                    ++r;
            else
                while (m++)
                    --r;
            return r;
        }();
        r = initialIterator;
        auto&& actualResult = r += n;
        EXPECT_EQ(actualResult, expectedResult);
        static_assert(std::is_same<decltype(actualResult), X&>::value, "Type of result 'r += n' tested");
    }
    {
        // Expressions to be tested: 'a + n' and 'n + a'
        difference_type n = 3;

        static_assert(std::is_same<decltype(a + n), X>::value, "Return type tested");
        static_assert(std::is_same<decltype(n + a), X>::value, "Return type tested");

        const auto expectedResult = [a, n] {
            // Operational semantics, as specified by the C++11 Standard:
            X tmp = a;
            return tmp += n;
        }();

        EXPECT_EQ(a + n, expectedResult);
        EXPECT_TRUE(a + n == n + a);
    }
    {
        // Expression to be tested: 'r -= n'
        difference_type n = 3;

        r = initialIterator;
        const auto expectedResult = [&r, n] {
            // Operational semantics, as specified by the C++11 Standard:
            return r += -n;
        }();
        r = initialIterator;
        auto&& actualResult = r -= n;
        EXPECT_EQ(actualResult, expectedResult);
        static_assert(std::is_same<decltype(actualResult), X&>::value, "Type of result 'r -= n' tested");
    }
    {
        // Expression to be tested: 'a - n'
        difference_type n = -3;

        static_assert(std::is_same<decltype(a - n), X>::value, "Return type tested");

        const auto expectedResult = [a, n] {
            // Operational semantics, as specified by the C++11 Standard:
            X tmp = a;
            return tmp -= n;
        }();

        EXPECT_EQ(a - n, expectedResult);
    }
    {
        // Expression to be tested: 'b - a'
        static_assert(std::is_same<decltype(b - a), difference_type>::value, "Return type tested");

        difference_type n = b - a;
        EXPECT_TRUE(a + n == b);
        EXPECT_TRUE(b == a + (b - a));
    }
    {
        // Expression to be tested: 'a[n]'
        difference_type n = 3;
        static_assert(std::is_convertible<decltype(a[n]), reference>::value, "Return type tested");
        EXPECT_EQ(a[n], *(a + n));
    }
    {
        // Expressions to be tested: 'a < b', 'a > b', 'a >= b', and 'a <= b':
        static_assert(std::is_convertible<decltype(a < b), bool>::value, "Return type tested");
        static_assert(std::is_convertible<decltype(a > b), bool>::value, "Return type tested");
        static_assert(std::is_convertible<decltype(a >= b), bool>::value, "Return type tested");
        static_assert(std::is_convertible<decltype(a <= b), bool>::value, "Return type tested");
        EXPECT_EQ(a < b, b - a> 0);
        EXPECT_EQ(a > b, b < a);
        EXPECT_EQ(a >= b, !(a < b));
        EXPECT_EQ(a <= b, !(b < a));
    }
}

