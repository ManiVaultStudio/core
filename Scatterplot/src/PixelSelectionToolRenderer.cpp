#include "PixelSelectionToolRenderer.h"
#include "PixelSelectionTool.h"

#include "util/FileUtil.h"

#include <QDebug>
#include <QFile>
#include <QPainter>
#include <QLabel>

namespace hdps
{
namespace gui
{

PixelSelectionToolRenderer::PixelSelectionToolRenderer(PixelSelectionTool& pixelSelectionTool) :
    _pixelSelectionTool(pixelSelectionTool),
    _renderSize(),
    _shaderProgram(QSharedPointer<QOpenGLShaderProgram>::create()),
    _areaTexture(QSharedPointer<QOpenGLTexture>::create(QOpenGLTexture::Target::Target2D)),
    _shapeTexture(QSharedPointer<QOpenGLTexture>::create(QOpenGLTexture::Target::Target2D)),
    _dummyVAO()
{
}

void PixelSelectionToolRenderer::init()
{
    try
    {
        if (!initializeOpenGLFunctions())
            throw std::runtime_error("Unable to initialize OpenGL functions");

        createShaderProgram();

        _dummyVAO.create();
    }
    catch (std::exception& e)
    {
        qDebug() << "Selection tool renderer initialization failed:" << e.what();
    }
    catch (...) {
        qDebug() << "Selection tool renderer initialization failed due to unhandled exception";
    }
}

void PixelSelectionToolRenderer::resize(QSize renderSize)
{
    if (renderSize == _renderSize)
        return;

    _renderSize = renderSize;
}

void PixelSelectionToolRenderer::render()
{
    if (!_areaTexture->isCreated() || !_shapeTexture->isCreated())
        return;

    try {
        if (!_shaderProgram->bind())
            throw std::runtime_error("Unable to bind shader program");

        glViewport(0, 0, _renderSize.width(), _renderSize.height());

        _shaderProgram->setUniformValue("overlayTexture", 0);

        _dummyVAO.bind();
        {
            _areaTexture->bind();
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

            _shapeTexture->bind();
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        }
        _dummyVAO.release();

        _shaderProgram->release();
        _shapeTexture->release();
    }
    catch (std::exception& e)
    {
        qDebug() << "Selection tool renderer: render failed:" << e.what();
    }
    catch (...) {
        qDebug() << "Selection tool renderer: render failed due to unhandled exception";
    }
}

void PixelSelectionToolRenderer::destroy()
{
    _dummyVAO.destroy();
}

void PixelSelectionToolRenderer::update()
{
    const auto areaImage    = _pixelSelectionTool.getAreaPixmap().toImage();
    const auto shapeImage   = _pixelSelectionTool.getShapePixmap().toImage();
    
    if (areaImage.isNull() || shapeImage.isNull())
        return;

    const auto updateTexture = [](QSharedPointer<QOpenGLTexture>& texture, const QImage& image) {
        const auto sizeChanged = image.size() != QSize(texture->width(), texture->height());

        if (!texture->isCreated() || sizeChanged) {
            texture.reset(new QOpenGLTexture(image));
            texture->create();
            texture->setWrapMode(QOpenGLTexture::ClampToBorder);
        }
        else {
            if (texture->isStorageAllocated())
                texture->setData(QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, image.bits());
        }
    };

    updateTexture(_areaTexture, areaImage);
    updateTexture(_shapeTexture, shapeImage);
}

void PixelSelectionToolRenderer::createShaderProgram()
{
    const auto selectionToolVert = hdps::util::loadFileContents(":shaders/SelectionTool.vert");

    if (selectionToolVert.isEmpty())
        throw std::runtime_error("Unable to load quad vertex shader");

    const auto selectionToolFrag = hdps::util::loadFileContents(":shaders/SelectionTool.frag");

    if (selectionToolFrag.isEmpty())
        throw std::runtime_error("Unable to load quad fragment shader");

    if (!_shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, selectionToolVert))
        throw std::runtime_error("Unable to compile quad vertex shader");

    if (!_shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, selectionToolFrag))
        throw std::runtime_error("Unable to compile quad fragment shader");

    if (!_shaderProgram->link())
        throw std::runtime_error("Unable to link quad shader program");

    _shaderProgram->release();
}

}
}