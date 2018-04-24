#pragma once

namespace hdps
{
    class Vector2f;
    class Selection;

    class Matrix3f
    {
    public:
        Matrix3f();
        Matrix3f(float m00, float m01, float m10, float m11, float m02, float m12);

        void setIdentity();

        void scale(const Vector2f& scale);

        float* toArray();

        float operator[](int i) const;
        float& operator[](int i);

        Matrix3f operator*(const Matrix3f& m) const;
        Vector2f operator*(const Vector2f& v) const;
        Selection operator*(const Selection& s) const;
    private:
        float a[9];
    };

} // namespace hdps
