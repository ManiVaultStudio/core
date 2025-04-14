// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Shader.h"

#include "graphics/Matrix3f.h"
#include "graphics/Vector3f.h"
#include "util/FileUtil.h"

//#include "graphics/Matrix4f.h"

#include <QDebug>
#include <QOpenGLVersionFunctionsFactory>

namespace mv
{

namespace
{
    const int LOG_SIZE = 1024;

    bool compileShader(QString path, GLuint shader)
    {
        QOpenGLFunctions_3_3_Core* f = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_3_3_Core>(QOpenGLContext::currentContext());;

        f->glCompileShader(shader);

        GLint status;
        
        f->glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE) {
            std::vector<GLchar> shaderLog(LOG_SIZE);
            f->glGetShaderInfoLog(shader, LOG_SIZE, nullptr, shaderLog.data());
            qCritical().noquote() << "Shader failed to compile: " << path << "\n" << shaderLog.data();
            return false;
        }
        return true;
    }

    bool linkProgram(const GLuint program)
    {
        QOpenGLFunctions_3_3_Core* f = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_3_3_Core>(QOpenGLContext::currentContext());;

        f->glLinkProgram(program);

        GLint status = 0;
        f->glGetProgramiv(program, GL_LINK_STATUS, &status);

        if (status != GL_TRUE) {
            GLint logLength = 0;
            f->glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
            
            std::vector<GLchar> infoLog(logLength);
            f->glGetProgramInfoLog(program, logLength, &logLength, infoLog.data());
            
            qCritical().noquote() << "[Shader linking] Shader program failed to link.";
            qCritical().noquote() << "[Shader linking] Linking Error message(Len" << logLength << ") :" << infoLog.data();
            qCritical().noquote() << "[Shader linking] Last OpenGL error code: " << f->glGetError();
            return false;
        }

        return true;
    }

    bool validateProgram(const GLuint program)
    {
        QOpenGLFunctions_3_3_Core* f = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_3_3_Core>(QOpenGLContext::currentContext());;

        f->glValidateProgram(program);

        GLint status = 0;
        f->glGetProgramiv(program, GL_VALIDATE_STATUS, &status);

        return status == GL_TRUE;
    }

    bool loadShader(QString path, int type, GLuint& shader)
    {
        QOpenGLFunctions_3_3_Core* f = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_3_3_Core>(QOpenGLContext::currentContext());;

        QString source = mv::util::loadFileContents(path);
        if (source.isEmpty()) return false;

        std::string ssource = source.toStdString();
        const char* csource = ssource.c_str();

        // Create the shader
        shader = f->glCreateShader(type);
        f->glShaderSource(shader, 1, &csource, NULL);
        
        return compileShader(path, shader);
    }
}

ShaderProgram::ShaderProgram() : 
    _handle(),
    _locationMap()
{
    
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(_handle);
}

void ShaderProgram::bind() {
    glUseProgram(_handle);
}

void ShaderProgram::release() {
    glUseProgram(0);
}

void ShaderProgram::destroy() {
    glDeleteProgram(_handle);
}

void ShaderProgram::uniform1i(const char* name, int value) {
    glUniform1i(location(name), value);
}

void ShaderProgram::uniform1iv(const char* name, int count, int* values) {
    glUniform1iv(location(name), count, (GLint*)values);
}

void ShaderProgram::uniform2i(const char* name, int v0, int v1) {
    glUniform2i(location(name), v0, v1);
}

void ShaderProgram::uniform1f(const char* name, float value) {
    glUniform1f(location(name), value);
}

void ShaderProgram::uniform2f(const char* name, float v0, float v1) {
    glUniform2f(location(name), v0, v1);
}

void ShaderProgram::uniform3f(const char* name, float v0, float v1, float v2) {
    glUniform3f(location(name), v0, v1, v2);
}

void ShaderProgram::uniform3f(const char* name, Vector3f v) {
    glUniform3f(location(name), v.x, v.y, v.z);
}

void ShaderProgram::uniform3fv(const char* name, int count, Vector3f* v) {
    glUniform3fv(location(name), count, (GLfloat*)v);
}

void ShaderProgram::uniform4f(const char* name, float v0, float v1, float v2, float v3) {
    glUniform4f(location(name), v0, v1, v2, v3);
}

void ShaderProgram::uniformMatrix3f(const char* name, Matrix3f& m) {
    glUniformMatrix3fv(location(name), 1, false, m.toArray());
}

//void Shader::uniformMatrix4f(const char* name, Matrix4f& m) {
//    glUniformMatrix4fv(location(name), 1, false, m.toArray());
//}

void ShaderProgram::uniformMatrix4f(const char* name, const float* const m)
{
    glUniformMatrix4fv(location(name), 1, false, m);
}

int ShaderProgram::location(const char* name) {
    std::unordered_map<std::string, int>::const_iterator it = _locationMap.find(std::string(name));
    if (it != _locationMap.end()) {
        return it->second;
    }
    else {
        int location = glGetUniformLocation(_handle, name);
        _locationMap[std::string(name)] = location;
        return location;
    }
}

bool ShaderProgram::loadShaderFromFile(QString vertPath, QString fragPath) {
    initializeOpenGLFunctions();

    bool success = true;

    GLuint vertexShader, fragmentShader;
    success &= loadShader(vertPath, GL_VERTEX_SHADER, vertexShader);
    success &= loadShader(fragPath, GL_FRAGMENT_SHADER, fragmentShader);

    if (!success) return false;

    _handle = glCreateProgram();

    glAttachShader(_handle, vertexShader);
    glAttachShader(_handle, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    success &= linkProgram(_handle);
    success &= validateProgram(_handle);

    return success;
}

}
