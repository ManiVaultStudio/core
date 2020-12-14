#pragma once

#include "renderers/Renderer.h"

#include <QPixmap>
#include <QSharedPointer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

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

public:

    void update();

private:

    /** Creates the shader program for displaying the selection tool overlay */
    void createShaderProgram();

private:
    SelectionTool&                          _selectionTool;         /** Selection tool to visualize */
    QSize                                   _renderSize;            /** Size of the renderer */
    QSharedPointer<QOpenGLShaderProgram>    _shaderProgram;         /** OpenGL shader programs */
    QSharedPointer<QOpenGLTexture>          _shapeTexture;          /** OpenGL texture for selection tool shape */
    QSharedPointer<QOpenGLTexture>          _areaTexture;           /** OpenGL texture for selected area */
};

}
}