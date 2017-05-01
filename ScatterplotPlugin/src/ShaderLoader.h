/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** ShaderLoader.h
** Declares a class for loading vertex and fragment shaders and compiling
** them into a shader program.
**
** Author: Julian Thijssen
** -------------------------------------------------------------------------*/

#pragma once
#ifndef SHADERLOADER_H
#define SHADERLOADER_H

class ShaderLoader {
public:
    static const int LOG_SIZE;

    static int loadShaderProgram();
private:
    static int loadShader(const char* source, int type);
    static void checkCompilationStatus(const int shader);
    static void checkLinkStatus(const int program);
};

#endif /* SHADERLOADER_H */
