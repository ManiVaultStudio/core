#ifndef TEXTURE_H
#define TEXTURE_H

#include <QOpenGLFunctions_3_3_Core>
#include <QDebug>
namespace hdps
{

class Texture2D : protected QOpenGLFunctions_3_3_Core
{
public:
    Texture2D() : _handle(0)
    {
        initializeOpenGLFunctions();

        glGenTextures(1, &_handle);
    }

    ~Texture2D()
    {
        glDeleteTextures(1, &_handle);
    }

    void bind()
    {
        glBindTexture(GL_TEXTURE_2D, _handle);
    }

    void bind(unsigned int textureUnit)
    {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        bind();
    }

    void release()
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

protected:
    GLuint _handle;
};

} // namespace hdps

#endif // TEXTURE_H
