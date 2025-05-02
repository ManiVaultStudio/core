#pragma once

#include <vector>
#include <stdexcept>
#include <exception>
#include <limits>
#include "biovault_bfloat16/biovault_bfloat16.h"

namespace std
{
    template<>
    class numeric_limits<biovault::bfloat16_t>
    {
    public:
        static constexpr auto is_specialized = true;
        static constexpr auto is_signed = true;
        static constexpr auto is_integer = false;
        static constexpr auto is_exact = false;
        static constexpr auto has_infinity = false;
        static constexpr auto has_quiet_NaN = numeric_limits<float>::has_quiet_NaN; // not sure, taking float value for now
        static constexpr auto has_signaling_NaN = numeric_limits<float>::has_signaling_NaN; // not sure, taking float value for now
        static constexpr auto has_denorm = numeric_limits<float>::has_denorm; // not sure, taking float value for now
        static constexpr auto round_style = numeric_limits<float>::round_style; // not sure, taking float value for now
        static constexpr auto is_iec559 = false;
        static constexpr auto is_bounded = numeric_limits<float>::is_bounded;
        static constexpr auto is_modulo = numeric_limits<float>::is_modulo;
        static constexpr auto digits = 8;
        static constexpr auto digits10 = 2;
        static constexpr auto max_digits10 = 4;
        static constexpr auto radix = numeric_limits<float>::radix;
        static constexpr auto min_exponent = numeric_limits<float>::min_exponent;
        static constexpr auto min_exponent10 = numeric_limits<float>::min_exponent10;
        static constexpr auto max_exponent = numeric_limits<float>::max_exponent;
        static constexpr auto max_exponent10 = numeric_limits<float>::max_exponent10;
        static constexpr auto traps = numeric_limits<float>::traps;;
        static constexpr auto tinyness_before = numeric_limits<float>::tinyness_before; // not sure, taking float value for now

        static constexpr auto min() { return biovault::bfloat16_t(0x0080, true); }           // not sure
        static constexpr auto lowest() { return biovault::bfloat16_t(0xff7f, true); }        // not sure
        static constexpr auto max() { return biovault::bfloat16_t(0x7f7f, true); }           // not sure  
        static constexpr auto epsilon() { return biovault::bfloat16_t(0x3c00, true); }       // not sure
        static constexpr auto round_error() { return biovault::bfloat16_t(0x3f00, true); }   // not sure
        static constexpr auto infinity() { return biovault::bfloat16_t(0x7f80, true); }      // not sure
        static constexpr auto quiet_NaN() { return biovault::bfloat16_t(0x7fc0, true); }     // not sure
        static constexpr auto signaling_NaN() { return biovault::bfloat16_t(0x7fa0, true); } // not sure
        static constexpr auto denorm_min() { return biovault::bfloat16_t(0x0001, true); }    // not sure

    };
}

namespace mv_matrix
{
    using row_size_type = std::uint32_t;
    using column_size_type = std::uint32_t;
    using index_size_type = std::uint64_t;

    template<typename T>
    constexpr const char* type_name() 
    { 
        T dummy;
        return nullptr;/* QMetaType::fromType<T>.name();*/ 
    }

    template<>
    constexpr const char* type_name<biovault::bfloat16_t>() { return  "Bfloat16"; }

    template<>
    constexpr const char* type_name<float>() { return  "Float32"; }
    template<>
    constexpr const char* type_name<double>() { return  "Float64"; }
    template<>
    constexpr const char* type_name<std::int8_t>() { return  "Int8"; }
    template<>
    constexpr const char* type_name<std::int16_t>() { return  "Int16"; }
    template<>
    constexpr const char* type_name<std::int32_t>() { return  "Int32"; }
    template<>
    constexpr const char* type_name<std::int64_t>() { return  "Int64"; }
    template<>
    constexpr const char* type_name<std::uint8_t>() { return  "Uint8"; }
    template<>
    constexpr const char* type_name<std::uint16_t>() { return  "Unt16"; }
    template<>
    constexpr const char* type_name<std::uint32_t>() { return  "Uint32"; }
    template<>
    constexpr const char* type_name<std::uint64_t>() { return  "Uint64"; }



    template <typename... Types>
    struct ListOfElementType
    {
        static constexpr std::array<const char*, sizeof...(Types)> Names = {
          type_name<Types>()
        };
    };


    template <typename T>
    struct ListOf;


    template <typename... Types>
    struct ListOf<std::variant<Types...>>
    {
        static constexpr std::array<const char*, sizeof...(Types)> MatrixTypeNames = {
            Types::matrix_type_name()...
        };

        static constexpr std::array<const char*, sizeof...(Types)> ElementTypeNames = {
            Types::data_type_name()...
        };
    };


    


    class MatrixException : public std::exception {
    private:
        const char* message;

    public:
        MatrixException(const char* msg) : message(msg) {}
        const char* what() {
            return message;
        }
    };



    template<typename T>
    std::size_t vectorSizeInBytes(const std::vector<T>& vec)
    {
        return (vec.size() * sizeof(T));
    }


    template<typename R, typename U>
    class IntegerCompareSpecialization
    {
    private:
        template<typename T>
        static constexpr bool UnsupportedTypes()
        {
            return !std::is_integral_v<T> || std::is_enum_v<T> || std::is_same_v<T, char> || std::is_same_v<T, char16_t> ||
                std::is_same_v<T, char32_t> || std::is_same_v<T, wchar_t> || std::is_same_v<T, bool> ||
                std::is_same_v<T, std::byte>;
        }
    public:
        static constexpr bool value = !(UnsupportedTypes<R>() || UnsupportedTypes<U>());
    };

    template<bool IntegerSpecialization, typename T, typename U>
    class compare
    {
    };

    template<typename T, typename U>
    class compare<false, T, U>
    {
    public:
        static constexpr bool in_range(U u_min, U u_max) noexcept
        {
            return ((u_min >= std::numeric_limits<T>::min()) && (u_max <= std::numeric_limits<T>::max()));
        }
    };
    template<typename T, typename U>
    class compare<true, T, U>
    {
    private:
        static constexpr  bool cmp_less(T t, U u) noexcept
        {
            if constexpr (std::is_signed_v<T> == std::is_signed_v<U>)
                return t < u;
            else if constexpr (std::is_signed_v<T>)
                return t < 0 || std::make_unsigned_t<T>(t) < u;
            else
                return u >= 0 && t < std::make_unsigned_t<U>(u);
        }

        static constexpr  bool cmp_greater(T t, U u) noexcept
        {
            return cmp_less(u, t);
        }

        static constexpr  bool cmp_less_equal(T t, U u) noexcept
        {
            return !cmp_less(u, t);
        }

        static constexpr bool cmp_greater_equal(T t, U u) noexcept
        {
            return !cmp_less(t, u);
        }

    public:
        static constexpr bool in_range(U u) noexcept
        {
            return cmp_greater_equal(u, std::numeric_limits<T>::min()) &&
                cmp_less_equal(u, std::numeric_limits<T>::max());
        }

        static constexpr bool in_range(U u_min, U u_max) noexcept
        {
            return cmp_greater_equal(u_min, std::numeric_limits<T>::min()) &&
                cmp_less_equal(u_max, std::numeric_limits<T>::max());
        }
    };

    template<typename R, typename T>
    constexpr bool in_range(T u_min, T u_max) noexcept
    {
        return compare<IntegerCompareSpecialization<R, T>::value, R, T>::in_range(u_min, u_max);
    }

    template< typename T, typename U>
    T safe_numeric_cast(const U& source)
    {
        static_assert(std::numeric_limits<T>::is_specialized);
        static_assert(std::numeric_limits<U>::is_specialized);

      
        if constexpr (std::is_floating_point_v<T>)
        {
            constexpr long double max_t = (double)std::numeric_limits<T>::max();
            constexpr long double lowest_t = (double)std::numeric_limits<T>::lowest();
            constexpr long double max_u = (double)std::numeric_limits<U>::max();
            constexpr long double lowest_u = (double)std::numeric_limits<U>::lowest();

            if constexpr ((max_t >= max_u) && (lowest_t <= lowest_u))
            {
                return static_cast<T>(source);
            }
        }
        else
        {
            static_assert(std::numeric_limits<U>::is_integer);
            if constexpr (std::numeric_limits<T>::is_signed || !std::numeric_limits<U>::is_signed)
            {
                if constexpr (in_range<T, U>(std::numeric_limits<U>::min(), std::numeric_limits<U>::max()))
                {
                    return static_cast<T>(source);
                }
            }
        }

        // if we cannot be certain source fits in T we check the actual value, but only in Debug mode for performance reasons
#ifdef _DEBUG
        constexpr long double max_t = (double)std::numeric_limits<T>::max();
        constexpr long double lowest_t = (double)std::numeric_limits<T>::lowest();
        if ((static_cast<long double>(source) >= lowest_t) && (static_cast<long double>(source) <= max_t))
            return static_cast<T>(source);
        throw std::out_of_range("invalid safe numerical cast");
#endif
        return static_cast<T>(source);

    };




}