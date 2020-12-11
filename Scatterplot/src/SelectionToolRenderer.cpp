#include "SelectionToolRenderer.h"
#include "SelectionTool.h"

#include "util/FileUtil.h"

#include <QDebug>
#include <QFile>
#include <QPainter>

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
    QObject::connect(&_selectionTool, &SelectionTool::geometryChanged, [this]() {
        QPainter painter(&_pixmap);

        _selectionTool.paint(&painter);
    });
}

void SelectionToolRenderer::init()
{
    try
    {
        qDebug() << hdps::util::loadFileContents(":shaders/SelectionToolVertex.glsl");

        if (!_shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, hdps::util::loadFileContents(":shaders/SelectionToolVertex.glsl")))
            throw std::runtime_error("Unable to compile quad vertex shader");

        if (!_shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, hdps::util::loadFileContents(":shaders/SelectionToolFragment.glsl")))
            throw std::runtime_error("Unable to compile quad fragment shader");

        if (!_shaderProgram->link())
            throw std::runtime_error("Unable to link quad shader program");

        const auto stride = 5 * sizeof(GLfloat);

        //shaderProgram->enableAttributeArray(QuadShape::_vertexAttribute);
        //shaderProgram->enableAttributeArray(QuadShape::_textureAttribute);
        //shaderProgram->setAttributeBuffer(QuadShape::_vertexAttribute, GL_FLOAT, 0, 3, stride);
        //shaderProgram->setAttributeBuffer(QuadShape::_textureAttribute, GL_FLOAT, 3 * sizeof(GLfloat), 2, stride);
        _shaderProgram->release();
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
}

void SelectionToolRenderer::render()
{
    try {
        //if (!_texture->isCreated())
            //throw std::runtime_error("Texture is not created");

        glActiveTexture(GL_TEXTURE0);

        //_texture->bind();

        if (!_shaderProgram->bind())
            throw std::runtime_error("Unable to bind shader program");

        /*
        auto selectionLayer = static_cast<SelectionLayer*>(_node);

        const auto overlayColor = selectionLayer->getOverlayColor(Qt::EditRole).value<QColor>();

        shaderProgram->setUniformValue("channelTextures", 0);
        shaderProgram->setUniformValue("textureSize", QSizeF(selectionLayer->getImageSize()));
        shaderProgram->setUniformValue("overlayColor", overlayColor);
        shaderProgram->setUniformValue("opacity", opacity);
        shaderProgram->setUniformValue("transform", nodeMVP);

        shape->render();
        */

        // draw a quad over the entire widget
        GLfloat vertices[]{ -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f };

        _shaderProgram->enableAttributeArray(0);
        _shaderProgram->setAttributeArray(0, GL_FLOAT, vertices, 2);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        
        _shaderProgram->disableAttributeArray(0);

        _shaderProgram->release();
        _texture->release();
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

}
}