/**
* @author Julian Thijssen
* 
* OpenGL Texture convenience classes.
* Encapsulates most of the functionality possible with OpenGL textures.
* If functionality is missing getHandle() can be called to retrieve
* the raw OpenGL texture ID.
*/

#pragma once

#include <QOpenGLFunctions_3_3_Core>

#include <QPixmap>
#include <QImage>
#include <QRgb>

namespace hdps
{

/**
* Base texture class. Contains most of the functionality,
* but can only be used through one of the sub-classes.
* The texture subclasses provide the specifics of the
* operations performed on the texture (such as dimensionality).
* It is wise to only call functions in this class when
* an OpenGL context is current and active, otherwise
* their behaviour is undefined.
*/
class Texture : protected QOpenGLFunctions_3_3_Core
{
public:
    /**
    * Texture constructor
    * @param  target  The GLenum specifying the target of texture operations.
    *                 GL_TEXTURE_1D, GL_TEXTURE_2D and GL_TEXTURE_3D are accepted.
    */
    Texture(GLenum target) :
        _created(false),
        _handle(0),
        _target(target)
    { }

    /**
    * Initializes the use of OpenGL functions in this context and
    * generates the raw OpenGL ID of the texture. This function
    * should be called before doing any other texture operations
    * and only when an OpenGL context is current and active.
    */
    void create()
    {
        initializeOpenGLFunctions();

        glGenTextures(1, &_handle);

        _created = true;
    }

    /**
    * Deletes the OpenGL texture ID and invalidates this texture
    * until create() is called again.
    */
    void destroy()
    {
        if (!_created) { return; }
        glDeleteTextures(1, &_handle);

        _created = false;
    }

    /**
    * Binds the texture to the given texture unit.
    * @param  textureUnit  The texture unit should be a value in the range [0-16).
    *                      One shouldn't try to pass OpenGL texture units in here,
    *                      as the given value gets added to GL_TEXTURE0.
    */
    void bind(unsigned int textureUnit)
    {
        if (!_created) { return; }
        if (textureUnit > MAX_TEXTURE_UNITS - 1) { return; }

        glActiveTexture(GL_TEXTURE0 + textureUnit);
        bind();
    }

    /**
    * Binds the texture to the active texture unit.
    */
    void bind()
    {
        if (!_created) { return; }

        glBindTexture(_target, _handle);
    }

    /**
    * Unbinds the texture by binding the default (0) texture.
    */
    void release()
    {
        glBindTexture(_target, 0);
    }

    /**
    * Return whether the texture has been allocated
    */
    bool isCreated()
    {
        return _created;
    }

    /**
    * Return the raw OpenGL texture ID.
    */
    GLuint getHandle()
    {
        return _handle;
    }

protected:
    // OpenGL 3.3 specifies that at least 16 texture units must be supported per stage.
    const unsigned int MAX_TEXTURE_UNITS = 16;

    bool _created;

    GLuint _handle;
    GLenum _target;
};

class Texture1D : public Texture
{
public:
    Texture1D() : Texture(GL_TEXTURE_1D) { }

    void loadFromFile(QString path)
    {
        QPixmap pixmap(path);

        QImage image = pixmap.toImage();
        QRgb* data = (QRgb*)image.bits();

        create();
        bind();
        glTexImage1D(_target, 0, GL_RGBA8, image.width()*image.height(), 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, data);

        glTexParameteri(_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        release();
    }
};

class Texture2D : public Texture
{
public:
    Texture2D() : Texture(GL_TEXTURE_2D) { }

    void loadFromImage(QImage image)
    {
        QRgb* data = (QRgb*)image.bits();

        create();
        bind();
        glTexImage2D(_target, 0, GL_RGBA8, image.width(), image.height(), 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, data);

        glTexParameteri(_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        release();
    }

    void loadFromFile(QString path)
    {
        QPixmap pixmap(path);

        QImage image = pixmap.toImage();
        QRgb* data = (QRgb*) image.bits();

        create();
        bind();
        glTexImage2D(_target, 0, GL_RGBA8, image.width(), image.height(), 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, data);

        glTexParameteri(_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        release();
    }
};

} // namespace hdps
