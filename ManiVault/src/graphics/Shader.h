// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QOpenGLFunctions_3_3_Core>

#include <unordered_map>
#include <string>

namespace mv
{
class Vector3f;
class Matrix3f;
class Matrix4f;

class CORE_EXPORT ShaderProgram : protected QOpenGLFunctions_3_3_Core
{
public:
    ShaderProgram();
    ~ShaderProgram() override;
    void bind();
    void release();
    void destroy();

    void uniform1i(const char* name, int value);
    void uniform1iv(const char* name, int count, int* values);
    void uniform2i(const char* name, int v0, int v1);
    void uniform1f(const char* name, float value);
    void uniform2f(const char* name, float v0, float v1);
    void uniform3f(const char* name, float v0, float v1, float v2);
    void uniform3f(const char* name, Vector3f v);
    void uniform3fv(const char* name, int count, Vector3f* v);
    void uniform4f(const char* name, float v0, float v1, float v2, float v3);
    void uniformMatrix3f(const char* name, Matrix3f& m);

    void uniformMatrix3f(const char* name, float* data);

    void uniformMatrix4f(const char* name, float* data);

    //void uniformMatrix4f(const char* name, Matrix4f& m);
    void uniformMatrix4f(const char* name, const float* const m);

    bool loadShaderFromFile(QString vertPath, QString fragPath);
private:
    int location(const char* uniform);

    GLuint _handle;
    std::unordered_map<std::string, int> _locationMap;
};
}
