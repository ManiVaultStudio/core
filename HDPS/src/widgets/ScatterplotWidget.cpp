#include "ScatterplotWidget.h"

#include <vector>

#include <QDebug>

#define GLSL(version, shader)  "#version " #version "\n" #shader

const char *plotVertexSource = GLSL(330,
    uniform float pointSize;

    uniform bool selecting;
    uniform vec2 start;
    uniform vec2 end;

    in vec2 vertex;
    in vec2 position;
    in vec3 color;

    out vec2 pass_texCoords;
    out vec3 pass_color;

    bool inRect(vec2 position, vec2 start, vec2 end)
    {
        return position.x > start.x && position.x < end.x && position.y > start.y && position.y < end.y;
    }

    void main()
    {
        pass_color = color;

        if (selecting && inRect(position, start, end))
        {
            pass_color = vec3(1, 0.5, 0);
        }

        pass_texCoords = vertex;
        gl_Position = vec4(vertex * pointSize + position, 0, 1);
    }
);

const char *plotFragmentSource = GLSL(330,
    uniform float alpha;

    in vec2 pass_texCoords;
    in vec3 pass_color;

    out vec4 fragColor;

    void main()
    {
        float len = length(pass_texCoords);
        // If the fragment is outside of the circle discard it
        if (len > 1) discard;

        float edge = fwidth(len);
        float a = smoothstep(1, 1 - edge, len);
        fragColor = vec4(pass_color, a * alpha);
    }
);

const char *selectionVertexSource = GLSL(330,
    uniform vec2 start;
    uniform vec2 end;

    void main()
    {
        vec2 vertex;
        if (gl_VertexID == 0) vertex = vec2(start.x, start.y);
        if (gl_VertexID == 1) vertex = vec2(end.x, start.y);
        if (gl_VertexID == 2) vertex = vec2(start.x, end.y);
        if (gl_VertexID == 3) vertex = vec2(end.x, end.y);
        gl_Position = vec4(vertex, 0, 1);
    }
);

const char *selectionFragmentSource = GLSL(330,
    out vec4 fragColor;

    void main()
    {
        fragColor = vec4(0.5, 0.5, 0.5, 0.1f);
    }
);

namespace hdps
{
namespace gui
{

// Positions need to be passed as a pointer as we need to store them locally in order
// to be able to find the subset of data that's part of a selection. If passed
// by reference then we can upload the data to the GPU, but not store it in the widget.
void ScatterplotWidget::setData(const std::vector<float>* positions)
{
    _numPoints = (unsigned int) positions->size() / 2;
    _positions = positions;
    _colors.clear();
    _colors.resize(_numPoints * 3, 0.5f);

    _positionBuffer.bind();
    _positionBuffer.setData(*positions);
    _colorBuffer.bind();
    _colorBuffer.setData(_colors);
    update();
}

void ScatterplotWidget::setColors(const std::vector<float>& colors)
{
    _colors = colors;

    _colorBuffer.bind();
    _colorBuffer.setData(colors);
    update();
}

void ScatterplotWidget::setPointSize(const float size)
{
    _pointSize = size;
    update();
}

void ScatterplotWidget::setAlpha(const float alpha)
{
    _alpha = alpha;
    _alpha = _alpha > 1 ? 1 : _alpha;
    _alpha = _alpha < 0 ? 0 : _alpha;
}

void ScatterplotWidget::setPointScaling(PointScaling scalingMode)
{
    _scalingMode = scalingMode;
}

void ScatterplotWidget::addSelectionListener(const plugin::SelectionListener* listener)
{
    _selectionListeners.push_back(listener);
}

void ScatterplotWidget::initializeGL()
{
    initializeOpenGLFunctions();

    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &ScatterplotWidget::cleanup);

    glClearColor(1.0, 1.0, 1.0, 1.0);
    qDebug() << "Initializing scatterplot";

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    std::vector<float> vertices(
    {
        -1, -1,
        1, -1,
        -1, 1,
        -1, 1,
        1, -1,
        1, 1
    }
    );

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    BufferObject quad;
    quad.create();
    quad.bind();
    quad.setData(vertices);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    _positionBuffer.create();
    _positionBuffer.bind();
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribDivisor(1, 1);
    glEnableVertexAttribArray(1);

    _colorBuffer.create();
    _colorBuffer.bind();
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribDivisor(2, 1);
    glEnableVertexAttribArray(2);

    if (_numPoints > 0)
    {
        _positionBuffer.bind();
        _positionBuffer.setData(*_positions);
        _colorBuffer.bind();
        _colorBuffer.setData(_colors);
    }

    _shader = new QOpenGLShaderProgram();
    _shader->addShaderFromSourceCode(QOpenGLShader::Vertex, plotVertexSource);
    _shader->addShaderFromSourceCode(QOpenGLShader::Fragment, plotFragmentSource);
    _shader->link();

    _selectionShader = new QOpenGLShaderProgram();
    _selectionShader->addShaderFromSourceCode(QOpenGLShader::Vertex, selectionVertexSource);
    _selectionShader->addShaderFromSourceCode(QOpenGLShader::Fragment, selectionFragmentSource);
    _shader->link();
}

void ScatterplotWidget::resizeGL(int w, int h)
{
    _windowSize.setWidth(w);
    _windowSize.setHeight(h);
}

void ScatterplotWidget::paintGL()
{
    qDebug() << "Rendering scatterplot";
    glClear(GL_COLOR_BUFFER_BIT);

    QPointF ns(_selectionStart.x() < _selectionEnd.x() ? _selectionStart.x() : _selectionEnd.x(), _selectionStart.y() < _selectionEnd.y() ? _selectionStart.y() : _selectionEnd.y());
    QPointF ne(_selectionStart.x() < _selectionEnd.x() ? _selectionEnd.x() : _selectionStart.x(), _selectionStart.y() < _selectionEnd.y() ? _selectionEnd.y() : _selectionStart.y());

    QRectF selection(ns, ne);

    _shader->bind();
    if (_scalingMode == Relative)
    {
        _shader->setUniformValue("pointSize", _pointSize / 800);
    }
    else if (_scalingMode == Absolute)
    {
        _shader->setUniformValue("pointSize", _pointSize / _windowSize.width());
    }

    _shader->setUniformValue("alpha", _alpha);
    _shader->setUniformValue("selecting", _selecting);
    _shader->setUniformValue("start", selection.topLeft());
    _shader->setUniformValue("end", selection.bottomRight());
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, _numPoints);

    if (_selecting)
    {
        // Selection
        _selectionShader->bind();

        //qDebug() << ns << ne;
        _selectionShader->setUniformValue("start", selection.topLeft());
        _selectionShader->setUniformValue("end", selection.bottomRight());
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
}

void ScatterplotWidget::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "Mouse clicky";
    _selecting = true;

    _selectionStart = QPointF((float)event->x() / _windowSize.width(), 1 - ((float)event->y() / _windowSize.height()));
    _selectionStart = _selectionStart * 2 - QPointF(1, 1);
}

void ScatterplotWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (_selecting)
    {
        _selectionEnd = QPointF((float)event->x() / _windowSize.width(), 1 - ((float)event->y() / _windowSize.height()));
        _selectionEnd = _selectionEnd * 2 - QPointF(1, 1);
        update();
    }
}

void ScatterplotWidget::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug() << "Mouse releasey";
    _selecting = false;

    _selectionEnd = QPointF((float)event->x() / _windowSize.width(), 1 - ((float)event->y() / _windowSize.height()));
    _selectionEnd = _selectionEnd * 2 - QPointF(1, 1);

    QRectF selection(_selectionStart, _selectionEnd);
    onSelection(selection);
}

void ScatterplotWidget::onSelection(QRectF selection)
{
    update();

    std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < _numPoints; i++)
    {
        QPointF point((*_positions)[i * 2 + 0], (*_positions)[i * 2 + 1]);

        if (selection.contains(point))
        {
            indices.push_back(i);
        }
    }

    for (const plugin::SelectionListener* listener : _selectionListeners)
    {
        listener->onSelection(indices);
    }
}

void ScatterplotWidget::cleanup()
{
    qDebug() << "Deleting scatterplot widget, performing clean up...";
    makeCurrent();

    glDeleteVertexArrays(1, &_vao);
    _positionBuffer.destroy();
    _colorBuffer.destroy();
    delete _shader;
    delete _selectionShader;
}

} // namespace gui

} // namespace hdps
