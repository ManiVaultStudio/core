#include "SelectionToolRenderer.h"
#include "SelectionTool.h"

#include "util/FileUtil.h"

#include <QDebug>
#include <QFile>
#include <QPainter>
#include <QLabel>

namespace hdps
{
namespace gui
{

SelectionToolRenderer::SelectionToolRenderer(SelectionTool& selectionTool) :
    _selectionTool(selectionTool),
    _renderSize(),
    _shaderProgram(QSharedPointer<QOpenGLShaderProgram>::create()),
    _shapeTexture(QSharedPointer<QOpenGLTexture>::create(QOpenGLTexture::Target::Target2D)),
    _areaTexture(QSharedPointer<QOpenGLTexture>::create(QOpenGLTexture::Target::Target2D)),
    _dummyVAO()
{
    
}

void SelectionToolRenderer::init()
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

void SelectionToolRenderer::resize(QSize renderSize)
{
    if (renderSize == _renderSize)
        return;

    _renderSize = renderSize;
}

void SelectionToolRenderer::render()
{
    try {
        if (!_areaTexture->isCreated())
            throw std::runtime_error("Area texture is not created");

        if (!_shapeTexture->isCreated())
            throw std::runtime_error("Shape texture is not created");

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

void SelectionToolRenderer::destroy()
{
    _dummyVAO.destroy();
}

void SelectionToolRenderer::update()
{
    const auto shapeImage   = _selectionTool.getShapePixmap().toImage().convertToFormat(QImage::Format_RGBA8888);
    const auto areaImage    = _selectionTool.getAreaPixmap().toImage();
    
    if (shapeImage.isNull() || areaImage.isNull())
        return;

    const auto updateTexture = [](QSharedPointer<QOpenGLTexture>& texture, const QImage& image) {

    };

    if (_shapeTexture->isStorageAllocated()) {
        _shapeTexture->setData(QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, shapeImage.bits());
    } else {
        _shapeTexture.reset(new QOpenGLTexture(shapeImage));
        _shapeTexture->create();
        _shapeTexture->setWrapMode(QOpenGLTexture::ClampToBorder);
    }

    if (_areaTexture->isStorageAllocated()) {
        _areaTexture->setData(QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, areaImage.bits());
    }
    else {
        _areaTexture.reset(new QOpenGLTexture(areaImage));
        _areaTexture->create();
        _areaTexture->setWrapMode(QOpenGLTexture::ClampToBorder);
    }
}

void SelectionToolRenderer::createShaderProgram()
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