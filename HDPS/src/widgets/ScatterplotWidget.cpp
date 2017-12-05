#include "ScatterplotWidget.h"

#include <vector>

#include <QDebug>

#define GLSL(version, shader)  "#version " #version "\n" #shader

const char *plotVertexSource = GLSL(330,
    uniform float pointSize;
    uniform vec3 selectionColor;

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
        return position.x > start.x && position.x < end.x && position.y < start.y && position.y > end.y;
    }

    void main()
    {
        pass_color = color;

        if (selecting && inRect(position, start, end))
        {
            pass_color = selectionColor;
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

        switch (gl_VertexID) {
        case 0: vertex = vec2(start.x, start.y); break;
        case 1: vertex = vec2(end.x, start.y); break;
        case 2: vertex = vec2(start.x, end.y); break;
        case 3: vertex = vec2(end.x, end.y); break;
        }

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
void ScatterplotWidget::setData(const std::vector<Vector2f>* positions)
{
    _numPoints = (unsigned int)positions->size();
    _positions = positions;
    _colors.clear();
    _colors.resize(_numPoints, Vector3f(0.5f, 0.5f, 0.5f));

    _positionBuffer.bind();
    _positionBuffer.setData(*positions);
    _colorBuffer.bind();
    _colorBuffer.setData(_colors);
    update();
}

void ScatterplotWidget::setColors(const std::vector<Vector3f>& colors)
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

void ScatterplotWidget::setSelectionColor(const Vector3f selectionColor)
{
    _selectionColor = selectionColor;
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

    _shader = std::make_unique<QOpenGLShaderProgram>();
    _shader->addShaderFromSourceCode(QOpenGLShader::Vertex, plotVertexSource);
    _shader->addShaderFromSourceCode(QOpenGLShader::Fragment, plotFragmentSource);
    _shader->link();

    _selectionShader = std::make_unique<QOpenGLShaderProgram>();
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

    int w = _windowSize.width();
    int h = _windowSize.height();
    int size = w < h ? w : h;
    glViewport(w / 2 - size / 2, h / 2 - size / 2, size, size);

    Vector2f topLeft = _selection.topLeft();
    Vector2f bottomRight = _selection.bottomRight();

    _shader->bind();
    switch (_scalingMode) {
        case Relative: _shader->setUniformValue("pointSize", _pointSize / 800); break;
        case Absolute: _shader->setUniformValue("pointSize", _pointSize / _windowSize.width()); break;
    }

    _shader->setUniformValue("selectionColor", _selectionColor.x, _selectionColor.y, _selectionColor.z);
    _shader->setUniformValue("alpha", _alpha);
    _shader->setUniformValue("selecting", _selecting);
    _shader->setUniformValue("start", topLeft.x, topLeft.y);
    _shader->setUniformValue("end", bottomRight.x, bottomRight.y);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, _numPoints);

    if (_selecting)
    {
        glViewport(0, 0, w, h);

        // Selection
        _selectionShader->bind();
        _selectionShader->setUniformValue("start", topLeft.x, topLeft.y);
        _selectionShader->setUniformValue("end", bottomRight.x, bottomRight.y);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
}

void ScatterplotWidget::mousePressEvent(QMouseEvent *event)
{
    _selecting = true;

    Vector2f point(event->x(), event->y());
    _selection.setStart(toClipCoordinates(point));
}

void ScatterplotWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!_selecting) return;

    Vector2f point(event->x(), event->y());
    _selection.setEnd(toClipCoordinates(point));

    update();
}

void ScatterplotWidget::mouseReleaseEvent(QMouseEvent *event)
{
    _selecting = false;

    Vector2f point(event->x(), event->y());
    _selection.setEnd(toClipCoordinates(point));

    onSelection(_selection);
}

void ScatterplotWidget::onSelection(Selection selection)
{
    update();

    std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < _numPoints; i++)
    {
        Vector2f point = (*_positions)[i];
        point.x *= _windowSize.width() / _windowSize.height();

        if (selection.contains(point))
            indices.push_back(i);
    }

    for (const plugin::SelectionListener* listener : _selectionListeners)
        listener->onSelection(indices);
}

void ScatterplotWidget::cleanup()
{
    qDebug() << "Deleting scatterplot widget, performing clean up...";
    makeCurrent();

    glDeleteVertexArrays(1, &_vao);
    _positionBuffer.destroy();
    _colorBuffer.destroy();
}

Vector2f ScatterplotWidget::toClipCoordinates(Vector2f windowCoordinates) const
{
    int w = _windowSize.width();
    int h = _windowSize.height();
    int size = w < h ? w : h;
    float wAspect = (float) w / size;
    float hAspect = (float) h / size;

    windowCoordinates /= Vector2f(w, h);
    windowCoordinates.y = 1 - windowCoordinates.y;
    //return windowCoordinates * 2 - 1;
    /////

    windowCoordinates = windowCoordinates * 2 - 1;
    windowCoordinates *= Vector2f(wAspect, hAspect);
    windowCoordinates -= Vector2f((wAspect - 1) / 2, (hAspect - 1) / 2);
    return windowCoordinates;
}

} // namespace gui

} // namespace hdps
