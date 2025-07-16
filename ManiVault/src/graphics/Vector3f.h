// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <string>
#include <iostream>

namespace mv
{
    class CORE_EXPORT Vector3f
    {
    public:
        // C++ Rule of Zero: Implicitly defined destructor and copy member functions are fine for this class.

        Vector3f() = default;

        constexpr Vector3f(float x, float y, float z)
            : x(x), y(y), z(z)
        {
        }

        constexpr Vector3f(float f)
            : x(f), y(f), z(f)
        {
        }

        /**
        * Sets the x, y and z components of this vector to the given parameters.
        * @param x the x-component of the vector
        * @param y the y-component of the vector
        * @param z the z-component of the vector
        */
        void set(float x, float y, float z);

        /**
        * Adds the given vector to this vector.
        * @param v the vector to be added
        */
        void add(Vector3f v);

        /**
        * Subtracts the given vector from this vector.
        * @param v the vector to be subtracted
        */
        void sub(Vector3f v);

        /**
        * Returns the squared magnitude of this vector.
        * @return the squared magnitude of this vector
        */
        constexpr float sqrMagnitude() const
        {
            return x * x + y * y + z * z;
        }

        /**
        * Returns the magnitude of this vector.
        * @return the magnitude of this vector
        */
        float length();

        /**
        * Returns a string representation of this vector.
        * @return the string representation
        */
        std::string str() const;

    public: /* Serialization */
        QVariantMap toVariantMap() const;
        void fromVariantMap(const QVariantMap& map);

    public: /* Operator overloads */
        constexpr bool operator==(const Vector3f& v) const
        {
            return x == v.x && y == v.y && z == v.z;
        }

        constexpr bool operator!=(const Vector3f& v) const
        {
            return x != v.x || y != v.y || z != v.z;
        }

        Vector3f& operator+=(const Vector3f& v);
        Vector3f& operator-=(const Vector3f& v);
        Vector3f& operator*=(const Vector3f& v);
        Vector3f& operator*=(float scale);
        Vector3f& operator/=(const Vector3f& v);

        constexpr Vector3f operator+(const Vector3f& v) const
        {
            return Vector3f(x + v.x, y + v.y, z + v.z);
        }

        constexpr Vector3f operator-(const Vector3f& v) const
        {
            return Vector3f(x - v.x, y - v.y, z - v.z);
        }

        constexpr Vector3f operator-() const
        {
            return Vector3f(-x, -y, -z);
        }

        constexpr Vector3f operator*(const Vector3f& v) const
        {
            return Vector3f(x * v.x, y * v.y, z * v.z);
        }

        constexpr Vector3f operator*(float scale) const
        {
            return Vector3f(x * scale, y * scale, z * scale);
        }

        constexpr Vector3f operator/(float divisor) const
        {
            return Vector3f(x / divisor, y / divisor, z / divisor);
        }

        float x{}, y{}, z{};
    };

    CORE_EXPORT std::ostream& operator<<(std::ostream& os, const Vector3f& v);

    CORE_EXPORT Vector3f min(Vector3f a, Vector3f b);
    CORE_EXPORT Vector3f max(Vector3f a, Vector3f b);

} // namespace mv
