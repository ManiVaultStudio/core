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

std::uint32_t SelectionToolRenderer::VERTEX_ATTRIBUTE_LOCATION = 0;
std::uint32_t SelectionToolRenderer::UV_ATTRIBUTE_LOCATION = 1;

SelectionToolRenderer::SelectionToolRenderer(SelectionTool& selectionTool) :
    _selectionTool(selectionTool),
    _renderSize(),
    _pixmap(),
    _shaderProgram(QSharedPointer<QOpenGLShaderProgram>::create()),
    _texture(QSharedPointer<QOpenGLTexture>::create(QOpenGLTexture::Target::Target2D)),
    _vao(),
    _vbo(),
    _vertexData()
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
        if (!initializeOpenGLFunctions())
            throw std::runtime_error("Unable to initialize OpenGL functions");

        _vao.create();
        _vbo.create();
        _vertexData.resize(20);

        _vao.bind();
        {
            _vbo.bind();
            {
                _vbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
                _vbo.allocate(_vertexData.constData(), _vertexData.count() * sizeof(GLfloat));
            }
            _vbo.release();
        }

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

        _vao.bind();
        _vbo.bind();

        const auto stride = 5 * sizeof(GLfloat);

        _shaderProgram->enableAttributeArray(VERTEX_ATTRIBUTE_LOCATION);
        _shaderProgram->enableAttributeArray(UV_ATTRIBUTE_LOCATION);
        _shaderProgram->setAttributeBuffer(VERTEX_ATTRIBUTE_LOCATION, GL_FLOAT, 0, 3, stride);
        _shaderProgram->setAttributeBuffer(UV_ATTRIBUTE_LOCATION, GL_FLOAT, 3 * sizeof(GLfloat), 2, stride);

        if (!_shaderProgram->link())
            throw std::runtime_error("Unable to link quad shader program");

        _vao.release();
        _vbo.release();
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

    const auto left     = 0.0f;
    const auto right    = static_cast<float>(_renderSize.width());
    const auto bottom   = static_cast<float>(_renderSize.height());
    const auto top      = 0.0f;

    const float coordinates[4][3] = {
        { left,		top,	0.0f },
        { right,	top,	0.0f },
        { right,	bottom,	0.0f },
        { left,		bottom,	0.0f }
    };

    for (int j = 0; j < 4; ++j)
    {
        _vertexData[j * 5 + 0] = coordinates[j][0];
        _vertexData[j * 5 + 1] = coordinates[j][1];
        _vertexData[j * 5 + 2] = coordinates[j][2];

        _vertexData[j * 5 + 3] = j == 0 || j == 3;
        _vertexData[j * 5 + 4] = j == 2 || j == 3;
    }

    _vbo.bind();
    _vbo.allocate(_vertexData.constData(), _vertexData.count() * sizeof(GLfloat));
    _vbo.release();
}

void SelectionToolRenderer::render()
{
    try {
        //if (!_texture->isCreated())
            //throw std::runtime_error("Texture is not created");

        //glActiveTexture(GL_TEXTURE0);

        //_texture->bind();

        if (!_shaderProgram->bind())
            throw std::runtime_error("Unable to bind shader program");

        //glViewport(0, 0, _renderSize.width(), _renderSize.height());

        _vao.bind();
        {
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        }
        _vao.release();

        _shaderProgram->release();

        //_texture->release();
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
    _vao.destroy();
    _vao.release();

    _vbo.destroy();
    _vbo.release();
}

void SelectionToolRenderer::createQuad()
{

}

void SelectionToolRenderer::updateQuad()
{

}

}
}