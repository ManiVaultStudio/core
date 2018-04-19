#ifndef MATRIX3F_H
#define MATRIX3F_H

#include "Vector2f.h"

#include <cmath>

namespace hdps
{
    class Matrix3f
    {
    public:
        Matrix3f() {
            setIdentity();
        }

        Matrix3f(float m00, float m01, float m10, float m11, float m02, float m12) {
            setIdentity();
            a[0] = m00;
            a[1] = m01;
            a[3] = m10;
            a[4] = m11;
            a[6] = m02;
            a[7] = m12;
        }

        void setIdentity() {
            for (int i = 0; i < 9; i++) {
                a[i] = i % 4 == 0 ? 1 : 0;
            }
        }

        void scale(const Vector2f& scale) {
            a[0] *= scale.x;
            a[1] *= scale.x;
            a[2] *= scale.x;
            a[3] *= scale.y;
            a[4] *= scale.y;
            a[5] *= scale.y;
        }

        float* toArray() {
            return a;
        }

        float operator[](int i) const {
            return a[i];
        }

        float& operator[](int i) {
            return a[i];
        }

        Matrix3f operator*(const Matrix3f& m) const {
            Matrix3f dest;
            dest.a[0] = a[0] * m[0] + a[3] * m[1] + a[6] * m[2];
            dest.a[1] = a[1] * m[0] + a[4] * m[1] + a[7] * m[2];
            dest.a[2] = a[2] * m[0] + a[5] * m[1] + a[8] * m[2];

            dest.a[3] = a[0] * m[3] + a[3] * m[4] + a[6] * m[5];
            dest.a[4] = a[1] * m[3] + a[4] * m[4] + a[7] * m[5];
            dest.a[5] = a[2] * m[3] + a[5] * m[4] + a[8] * m[5];

            dest.a[6] = a[0] * m[6] + a[3] * m[7] + a[6] * m[8];
            dest.a[7] = a[1] * m[6] + a[4] * m[7] + a[7] * m[8];
            dest.a[8] = a[2] * m[6] + a[5] * m[7] + a[8] * m[8];

            return dest;
        }

        Vector2f operator*(const Vector2f& v) const {
            Vector2f dest;
            dest.x = a[0] * v.x + a[3] * v.y + a[6];
            dest.y = a[1] * v.x + a[4] * v.y + a[7];

            return dest;
        }
    private:

        float a[9];
    };

} // namespace hdps

#endif // MATRIX3F_H
