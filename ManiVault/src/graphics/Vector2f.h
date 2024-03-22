// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <string>
#include <iostream>

namespace mv
{
    class CORE_EXPORT Vector2f
    {
    public:
        // C++ Rule of Zero: Implicitly defined destructor and copy member functions are fine for this class.

        Vector2f() = default;

        constexpr Vector2f(float x, float y)
            : x(x), y(y)
        {
        }

        /**
        * Sets the x and y components of this vector to the given parameters.
        * @param x the x-component of the vector
        * @param y the y-component of the vector
        */
        void set(float x, float y);

        /**
        * Adds the given vector to this vector.
        * @param v the vector to be added
        */
        void add(Vector2f v);

        /**
        * Subtracts the given vector from this vector.
        * @param v the vector to be subtracted
        */
        void sub(Vector2f v);

        /**
        * Returns the squared magnitude of this vector.
        * @return the squared magnitude of this vector
        */
        constexpr float sqrMagnitude() const
        {
            return x * x + y * y;
        }


        /**
        * Returns the magnitude of this vector.
        * @return the magnitude of this vector
        */
        float length() const;

        /**
        * Returns a string representation of this vector.
        * @return the string representation
        */
        std::string str() const;

        /* Operator overloads */
        constexpr bool operator==(const Vector2f& v) const
        {
            return x == v.x && y == v.y;
        }

        constexpr bool operator!=(const Vector2f& v) const
        {
            return x != v.x || y != v.y;
        }

        Vector2f& operator+=(const Vector2f& v);
        Vector2f& operator-=(const Vector2f& v);
        Vector2f& operator*=(const Vector2f& v);
        Vector2f& operator*=(float scale);
        Vector2f& operator/=(const Vector2f& v);

        constexpr Vector2f operator+(const Vector2f& v) const
        {
            return Vector2f(x + v.x, y + v.y);
        }

        constexpr Vector2f operator+(float value) const
        {
            return Vector2f(x + value, y + value);
        }

        constexpr Vector2f operator-(const Vector2f& v) const
        {
            return Vector2f(x - v.x, y - v.y);
        }

        constexpr Vector2f operator-(float value) const
        {
            return Vector2f(x - value, y - value);
        }

        constexpr Vector2f operator-() const
        {
            return Vector2f(-x, -y);
        }

        constexpr Vector2f operator*(const Vector2f& v) const
        {
            return Vector2f(x * v.x, y * v.y);
        }

        constexpr Vector2f operator*(float scale) const
        {
            return Vector2f(x * scale, y * scale);
        }

        constexpr Vector2f operator/(float divisor) const
        {
            return Vector2f(x / divisor, y / divisor);
        }

        float x{}, y{};
    };

    CORE_EXPORT float dot(const Vector2f& v1, const Vector2f& v2);
    CORE_EXPORT Vector2f normalize(const Vector2f& v);

    CORE_EXPORT std::ostream& operator<<(std::ostream& os, const Vector2f& v);

} // namespace mv
