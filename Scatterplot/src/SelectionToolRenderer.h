#pragma once

#include "renderers/Renderer.h"

#include <QPixmap>
#include <QSharedPointer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

class SelectionTool;

namespace hdps
{
namespace gui
{

class SelectionToolRenderer : protected Renderer
{
public:
    SelectionToolRenderer(SelectionTool& selectionTool);

public:
    void init() override;
    void resize(QSize renderSize) override;
    void render() override;
    void destroy() override;

private:

    /** Creates the necessary OpenGL buffer objects and performs initialization */
    void createQuad();

    /** Updates the quad vertices after a resize */
    void updateQuad();

    /** Creates the shader program for displaying the selection tool overlay */
    void createShaderProgram();

    void updateTexture();

private:
    SelectionTool&                          _selectionTool;         /** Selection tool to visualize */
    QSize                                   _renderSize;            /** Size of the renderer */
    QPixmap                                 _pixmap;                /** Pixmap for the overlay */
    QSharedPointer<QOpenGLShaderProgram>	_shaderProgram;         /** OpenGL shader programs */
    QSharedPointer<QOpenGLTexture>          _texture;               /** OpenGL texture */
    QOpenGLVertexArrayObject                _vao;                   /** OpenGL Vertex Array Object (VAO) */
    QOpenGLBuffer                           _vbo;                   /** OpenGL Vertex Buffer Object (VBO) */
    QVector<GLfloat>                        _vertexData;            /** Quad vertex data */

public:
    static std::uint32_t	VERTEX_ATTRIBUTE_LOCATION;		/** Quad vertex attribute location */
    static std::uint32_t	UV_ATTRIBUTE_LOCATION;		/** Quad texture attribute location */
};

}
}