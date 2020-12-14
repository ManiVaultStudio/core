#include "SelectionToolRenderer.h"
#include "SelectionTool.h"

#include "util/FileUtil.h"

#include <QDebug>
#include <QFile>
#include <QPainter>
#include <QLabel>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsBlurEffect>

namespace hdps
{
namespace gui
{

SelectionToolRenderer::SelectionToolRenderer(SelectionTool& selectionTool) :
    _selectionTool(selectionTool),
    _renderSize(),
    _pixmap(),
    _shaderProgram(QSharedPointer<QOpenGLShaderProgram>::create()),
    _texture(QSharedPointer<QOpenGLTexture>::create(QOpenGLTexture::Target::Target2D))
{
}

void SelectionToolRenderer::init()
{
    try
    {
        if (!initializeOpenGLFunctions())
            throw std::runtime_error("Unable to initialize OpenGL functions");

        createShaderProgram();
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

    _pixmap = QPixmap(_renderSize);

    _pixmap.fill(Qt::transparent);

    updateTexture();
}

void SelectionToolRenderer::render()
{
    try {
        glEnable(GL_TEXTURE_2D);
        {
            if (!_texture->isCreated())
                throw std::runtime_error("Texture is not created");

            if (!_shaderProgram->bind())
                throw std::runtime_error("Unable to bind shader program");

            _texture->bind();

            _shaderProgram->setUniformValue("overlayTexture", 0);

            glViewport(0, 0, _renderSize.width(), _renderSize.height());
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

            _shaderProgram->release();
            _texture->release();
        }
        glDisable(GL_TEXTURE_2D);
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

void SelectionToolRenderer::updateTexture()
{
    if (_pixmap.isNull())
        return;

    QPainter painter(&_pixmap);

    painter.setRenderHint(QPainter::Antialiasing);
    //painter.setRenderHint(QPainter::HighQualityAntialiasing);

    _pixmap.fill(Qt::transparent);

    _selectionTool.paint(&painter);

    _texture.reset(new QOpenGLTexture(_pixmap.toImage()));

    if (!_texture->isCreated())
        _texture->create();

    _texture->setWrapMode(QOpenGLTexture::ClampToBorder);
}

}
}