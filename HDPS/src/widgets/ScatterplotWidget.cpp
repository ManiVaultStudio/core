#include "ScatterplotWidget.h"

#include <vector>

#include <QDebug>

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

    bool loaded = true;
    loaded &= _shader.loadShaderFromFile(":shaders/PointPlot.vert", ":shaders/PointPlot.frag");
    loaded &= _selectionShader.loadShaderFromFile(":shaders/SelectionBox.vert", ":shaders/Color.frag");
    if (!loaded) {
        qDebug() << "Failed to load one of the Scatterplot shaders";
    }
}

void ScatterplotWidget::resizeGL(int w, int h)
{
    _windowSize.setWidth(w);
    _windowSize.setHeight(h);

    int size = w < h ? w : h;

    float wAspect = (float)w / size;
    float hAspect = (float)h / size;
    float wDiff = ((wAspect - 1) / 2.0);
    float hDiff = ((hAspect - 1) / 2.0);

    toNormalisedCoordinates = Matrix3f(1.0f / w, 0, 0, 1.0f / h, 0, 0);
    toIsotropicCoordinates = Matrix3f(wAspect, 0, 0, hAspect, -wDiff, -hDiff);
}

void ScatterplotWidget::paintGL()
{
    qDebug() << "Rendering scatterplot";
    glClear(GL_COLOR_BUFFER_BIT);

    int w = _windowSize.width();
    int h = _windowSize.height();
    int size = w < h ? w : h;
    glViewport(w / 2 - size / 2, h / 2 - size / 2, size, size);

    Vector2f topLeft = toClipCoordinates * toIsotropicCoordinates * _selection.topLeft();
    Vector2f bottomRight = toClipCoordinates * toIsotropicCoordinates * _selection.bottomRight();

    _shader.bind();
    switch (_scalingMode) {
        case Relative: _shader.uniform1f("pointSize", _pointSize / 800); break;
        case Absolute: _shader.uniform1f("pointSize", _pointSize / _windowSize.width()); break;
    }

    _shader.uniform3f("selectionColor", _selectionColor.x, _selectionColor.y, _selectionColor.z);
    _shader.uniform1f("alpha", _alpha);
    _shader.uniform1i("selecting", _selecting);
    _shader.uniform2f("start", topLeft.x, topLeft.y);
    _shader.uniform2f("end", bottomRight.x, bottomRight.y);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, _numPoints);

    if (_selecting)
    {
        topLeft = toClipCoordinates * _selection.topLeft();
        bottomRight = toClipCoordinates * _selection.bottomRight();

        glViewport(0, 0, w, h);

        // Selection
        _selectionShader.bind();
        _selectionShader.uniform2f("start", topLeft.x, topLeft.y);
        _selectionShader.uniform2f("end", bottomRight.x, bottomRight.y);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    qDebug() << "Done rendering scatterplot";
}

void ScatterplotWidget::mousePressEvent(QMouseEvent *event)
{
    _selecting = true;

    Vector2f point = toNormalisedCoordinates * Vector2f(event->x(), _windowSize.height() - event->y());
    _selection.setStart(point);
}

void ScatterplotWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!_selecting) return;

    Vector2f point = toNormalisedCoordinates * Vector2f(event->x(), _windowSize.height() - event->y());
    _selection.setEnd(point);

    update();
}

void ScatterplotWidget::mouseReleaseEvent(QMouseEvent *event)
{
    _selecting = false;

    Vector2f point = toNormalisedCoordinates * Vector2f(event->x(), _windowSize.height() - event->y());
    _selection.setEnd(point);

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

} // namespace gui

} // namespace hdps
