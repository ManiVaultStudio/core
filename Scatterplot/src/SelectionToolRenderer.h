#pragma once

#include "renderers/Renderer.h"

#include <QPixmap>
#include <QSharedPointer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>

class PixelSelectionTool;

namespace hdps
{
namespace gui
{

class SelectionToolRenderer : protected Renderer
{
public:
    SelectionToolRenderer(PixelSelectionTool& selectionTool);

public:
    void init() override;
    void resize(QSize renderSize) override;
    void render() override;
    void destroy() override;

public:

    void update();

private:

    /** Creates the shader program for displaying the selection tool overlay */
    void createShaderProgram();

private:
    PixelSelectionTool&                          _selectionTool;         /** Selection tool to visualize */
    QSize                                   _renderSize;            /** Size of the renderer */
    QSharedPointer<QOpenGLShaderProgram>    _shaderProgram;         /** OpenGL shader programs */
    QSharedPointer<QOpenGLTexture>          _areaTexture;           /** OpenGL texture for selected area */
    QSharedPointer<QOpenGLTexture>          _shapeTexture;          /** OpenGL texture for selection tool shape */
    QOpenGLVertexArrayObject                _dummyVAO;              /** Dummy Vertex Array Object*/
};

}
}