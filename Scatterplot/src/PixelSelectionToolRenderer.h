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

/**
 * Pixel selection tool renderer class
 *
 * Draws a pixel selection tool as an texture overlay with OpenGL
 *
 * @author Thomas Kroes
 */
class PixelSelectionToolRenderer : protected Renderer
{
public:

    /**
     * Constructor
     * @param pixelSelectionTool The reference to the pixel selection tool is used for the overlay
     */
    PixelSelectionToolRenderer(PixelSelectionTool& pixelSelectionTool);

public:
    void init() override;
    void resize(QSize renderSize) override;
    void render() override;
    void destroy() override;

public:

    /** Updates the textures for the overlay */
    void update();

private:

    /** Creates the shader program for displaying the selection tool overlay */
    void createShaderProgram();

private:
    PixelSelectionTool&                     _pixelSelectionTool;    /** Pixel selection tool for the overlay */
    QSize                                   _renderSize;            /** Size of the renderer */
    QSharedPointer<QOpenGLShaderProgram>    _shaderProgram;         /** OpenGL shader program */
    QSharedPointer<QOpenGLTexture>          _areaTexture;           /** OpenGL texture for the pixel selection overlay */
    QSharedPointer<QOpenGLTexture>          _shapeTexture;          /** OpenGL texture for the tool shape overlay */
    QOpenGLVertexArrayObject                _dummyVAO;              /** Dummy Vertex Array Object*/
};

}
}