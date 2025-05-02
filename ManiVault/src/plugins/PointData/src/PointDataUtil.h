#pragma once

#include <cstdint>




namespace mv
{
    namespace pointutils
    {
        template<typename T>
        bool is_integer(T value)
        {
            return std::is_integral_v<T> || (value == std::round(value));
        }

        template<typename T>
        bool contains_only_integers(const std::vector<T>& data)
        {
            if (std::is_integral_v<T>)
                return true;

            for (auto i = 0; i < data.size(); ++i)
            {
                if (!is_integer(data[i]))
                    return false;
            }
            return true;
        }
    }
}