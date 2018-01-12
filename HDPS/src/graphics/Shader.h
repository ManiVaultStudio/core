#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <QOpenGLFunctions_3_3_Core>

#include <unordered_map>
#include <string>

namespace hdps
{
class Vector3f;
class Matrix4f;

class ShaderProgram : protected QOpenGLFunctions_3_3_Core
{
public:
    ShaderProgram();
    ~ShaderProgram();
    void bind();
    void release();

    void uniform1i(const char* name, int value);
    void uniform1iv(const char* name, int count, int* values);
    void uniform2i(const char* name, int v0, int v1);
    void uniform1f(const char* name, float value);
    void uniform2f(const char* name, float v0, float v1);
    void uniform3f(const char* name, float v0, float v1, float v2);
    void uniform3f(const char* name, Vector3f v);
    void uniform3fv(const char* name, int count, Vector3f* v);
    void uniformMatrix4f(const char* name, Matrix4f& m);

    bool loadShaderFromFile(QString vertPath, QString fragPath);
private:
    int location(const char* uniform);

    GLuint _handle;
    std::unordered_map<std::string, int> _locationMap;
};
}

#endif /* SHADER_H */
