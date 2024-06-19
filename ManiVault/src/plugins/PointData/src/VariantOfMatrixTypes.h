#pragma once
#include <variant>
#include "DenseMatrix.h"
#include "CSRMatrix.h"

namespace mv
{

   

    
    template<class T, template<class> class U>
    inline constexpr bool is_instance_of_v = std::false_type{};

    template<template<class> class U, class V>
    inline constexpr bool is_instance_of_v<U<V>, U> = std::true_type{};
    
    
    template<class T, template<class...> class U>
    inline constexpr bool is_instance2_of_v = std::false_type{};

    template<template<class...> class U, class... Vs>
    inline constexpr bool is_instance2_of_v<U<Vs...>, U> = std::true_type{};
    

    

    template<typename ...Args>
    struct VariantOfMatrixTypes : public std::variant<mv::DenseMatrix<Args>..., mv::CSRMatrix<Args>...>
    {
        using ParentType = std::variant<mv::DenseMatrix<Args>..., mv::CSRMatrix<Args>...>;

        enum class MatrixType { Dense, CSR };

        static constexpr std::size_t NumberOfElementTypes = sizeof...(Args);

        template <std::size_t N>
        using ElementTypeAt = typename std::variant_alternative_t<N, ParentType>::value_type;
        
        constexpr  MatrixType getMatrixType() const
        {
            return (this->index() < NumberOfElementTypes) ? MatrixType::Dense : MatrixType::CSR;
        }

        bool isDense() const
        {
            return (this->index() < NumberOfElementTypes);
        }

        bool isCSR() const
        {
            return (this->index() >= NumberOfElementTypes);
        }

        constexpr std::size_t getElementTypeIndex() const
        {
            return (this->index() % NumberOfElementTypes);
        }

        // Sets the index of the specified variant. If the new index is different from the previous one, the value will be reset. 
  // Inspired by `expand_type` from kmbeutel at
  // https://www.reddit.com/r/cpp/comments/f8cbzs/creating_stdvariant_based_on_index_at_runtime/?rdt=52905
        template <typename... Alternatives>
        static void _setIndexOfVariant(std::variant<Alternatives...>& var, std::size_t index)
        {
            if (index != var.index())
            {
                assert(index < sizeof...(Alternatives));
                const std::variant<Alternatives...> variants[] = { Alternatives{ }... };
                var = std::move(variants[index]);
            }
        }

        // Returns the index of the specified alternative: the position of the alternative type withing the variant.
        // Inspired by `variant_index` from Bargor at
        // https://stackoverflow.com/questions/52303316/get-index-by-type-in-stdvariant
        template<typename Alternative, std::size_t index = 0>
        static constexpr std::size_t getIndexOfType()
        {
            static_assert(index < std::variant_size_v<ParentType>);

            if constexpr (std::is_same_v<std::variant_alternative_t<index, ParentType>,Alternative>)
            {
                return index;
            }
            else
            {
                return getIndexOfType<Alternative, index + 1>();
            }
        }

        void setIndexOfVariant(std::size_t idx)
        {
            _setIndexOfVariant(*this, idx);
        }

        void setAsDenseMatrixOfElementType(std::size_t elementTypeIdx)
        {
            assert(elementTypeIdx < NumberOfElementTypes);
            setIndexOfVariant(elementTypeIdx);
        }

        void setAsCSRMatrixOfElementType(std::size_t elementTypeIdx)
        {
            assert(elementTypeIdx < NumberOfElementTypes);
            setIndexOfVariant(NumberOfElementTypes+elementTypeIdx);
        }

        template <typename T>
        void setData(const T* const data, const std::size_t numPoints, const std::size_t numDimensions)
        {
            setAsDenseMatrix<T>();

            std::visit([this, data, numPoints, numDimensions](auto& matrix)
                {
                    matrix.setData(data, numPoints, numDimensions);
                }, *this);
        }

        template<typename T>
        void setAsDenseMatrix()
        {
            setIndexOfVariant(getIndexOfType<mv::DenseMatrix<std::decay_t<T>>>());
        }

        template<typename T>
        void setAsCSRMatrix()
        {
            setIndexOfVariant(getIndexOfType<mv::CSRMatrix<std::decay_t<T>>>());
        }
    };
}

