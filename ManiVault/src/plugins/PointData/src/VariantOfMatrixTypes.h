#pragma once
#include <variant>
#include "DenseMatrix.h"
#include "CSRMatrix.h"
#include "MatrixUtils.h"
#include "MatrixConfiguration.h"
namespace mv_matrix
{


    template<typename ...Args>
    struct VariantOfMatrixTypes : public std::variant<DenseMatrix<Args>..., CSRMatrix<Args>...>
    {
        using ParentType = std::variant< DenseMatrix<Args>..., CSRMatrix<Args>...>;

        static constexpr std::size_t NumberOfElementTypes = sizeof...(Args);
        static constexpr std::size_t NumberOfMatrixTypes = std::variant_size_v < ParentType>/sizeof...(Args);


        static constexpr auto getMatrixTypeNames()
        {
            return mv_matrix::ListOf<ParentType>::MatrixTypeNames;
        }

        static constexpr auto getElementTypeNames()
        {
            return mv_matrix::ListOf<ParentType>::ElementTypeNames;
        }


        template <std::size_t N>
        using ElementTypeAt = typename std::variant_alternative_t<N, ParentType>::value_type;
        

        constexpr std::size_t getElementTypeIndex() const
        {
            return (this->index() % NumberOfElementTypes);
        }


    private:
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


        template<std::size_t index = 0>
        static std::size_t getIndexOfType(const Configuration &config)
        {
            if constexpr (index < std::variant_size_v<ParentType>)
            {
                if ((config.storageType() == std::variant_alternative_t<index, ParentType>::matrix_type_name()) && (config.dataType() == std::variant_alternative_t<index, ParentType>::data_type_name()))
                {
                    return index;
                }
                else
                {
                    return getIndexOfType<index + 1>(config);
                }
            }
            else
            {
                return index;
            }
        }


    public:
        
        void setIndexOfVariant(std::size_t idx)
        {
            _setIndexOfVariant(*this, idx);
        }

        template<typename T>
        void setMatrixType()
        {

            const char* s = T::matrix_type_name();
            const char* d = T::data_type_name();
            auto index = getIndexOfType(Configuration(T::matrix_type_name(), T::data_type_name()));
            setIndexOfVariant(index);
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

       

        bool supportsStorageConfiguration(const Configuration& config)
        {
            const auto index = getIndexOfType(config);
            return (index < std::variant_size_v<ParentType>);
        }

        bool setConfiguration(const Configuration &config)
        {
            const auto index = getIndexOfType(config);
            if (index < std::variant_size_v<ParentType>)
            {
                setIndexOfVariant(index);
                return true;
            }
            else
                return false;
        }


        const char* getMatrixTypeName() const
        {
            std::visit([](auto& matrix)
                {
                    return matrix.matrix_type_name();
                }, *this);
        }

        const char* getElementTypeName() const
        {
            std::visit([](auto& matrix)
                {
                    return matrix.data_type_name();
                }, *this);
        }


        QVariantMap toVariantMap() const
        {
            return std::visit([](auto& matrix) -> QVariantMap
                {
                    return {
                        {"MatrixType", matrix.matrix_type_name()},
                        {"DataType", matrix.data_type_name()},
                        {"Data", matrix.toVariantMap()}
                    };
                }, *this);
           
        }

        void fromVariantMap(const QVariantMap& variantMap)
        {
            mv::util::variantMapMustContain(variantMap, "MatrixType");
            mv::util::variantMapMustContain(variantMap, "DataType");
            mv::util::variantMapMustContain(variantMap, "Data");

            const auto matrixTypeName = variantMap["MatrixType"].toString().toStdString();
            const auto dataTypeName = variantMap["DataType"].toString().toStdString();

            const std::size_t index = getIndexOfType(Configuration(matrixTypeName, dataTypeName));
            assert(index < std::variant_size_v<ParentType>);
            if (index < std::variant_size_v<ParentType>)
            {
                _setIndexOfVariant(*this, index);
                const QVariantMap& data = variantMap["Data"].toMap();

                std::visit([&data](auto& matrix)
                    {

                        matrix.fromVariantMap(data);
                    }, *this);
            }
            else
            {
                // what do we do if it fails ?
            }
        }
    };


    using VariantOfMatrices = mv_matrix::VariantOfMatrixTypes<
        float,
        biovault::bfloat16_t,
        std::int32_t,
        std::uint32_t,
        std::int16_t,
        std::uint16_t,
        std::int8_t,
        std::uint8_t
    >;
}

