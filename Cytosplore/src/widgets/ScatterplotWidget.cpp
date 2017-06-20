#include "ScatterplotWidget.h"

#include <vector>

#include <QDebug>

#define GLSL(version, shader)  "#version " #version "\n" #shader

const char *plotVertexSource = GLSL(330,
    uniform float pointSize;

    uniform vec2 start;
    uniform vec2 end;

    in vec2 vertex;
    in vec2 position;
    in vec3 color;

    out vec2 pass_texCoords;
    out vec3 pass_color;

    void main()
    {
        if (position.x > start.x && position.x < end.x && position.y > start.y && position.y < end.y) {
            pass_color = vec3(1, 0.5, 0);
        }
        else {
            pass_color = color;
        }
        pass_texCoords = vertex;
        //pass_color = color;
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
    vec2 vertices[4] = vec2[](
    vec2(-1, -1),
    vec2(1, -1),
    vec2(-1, 1),
    vec2(1, 1)
    );

    uniform vec2 start;
    uniform vec2 end;

    void main()
    {
        vec2 vertex;// = vertices[gl_VertexID];
        if (gl_VertexID == 0) { vertex = vec2(start.x, start.y); }
        if (gl_VertexID == 1) { vertex = vec2(end.x, start.y); }
        if (gl_VertexID == 2) { vertex = vec2(start.x, end.y); }
        if (gl_VertexID == 3) { vertex = vec2(end.x, end.y); }
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
    numPoints = positions->size() / 2;
    this->positions = positions;
    this->colors.resize(numPoints * 3, 0.5f);

    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, numPoints * 2 * sizeof(float), positions->data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, numPoints * 3 * sizeof(float), colors.data(), GL_STATIC_DRAW);
    update();
}

void ScatterplotWidget::setColors(const std::vector<float>& colors) {
    this->colors = colors;

    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, numPoints * 3 * sizeof(float), colors.data(), GL_STATIC_DRAW);
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
    selectionListeners.push_back(listener);
}

void ScatterplotWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(1.0, 1.0, 1.0, 1.0);
    qDebug() << "Initializing scatterplot";

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    float vertices[6*2] = 
    {
        -1, -1,
         1, -1,
        -1,  1,
        -1,  1,
         1, -1,
         1,  1
    };

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &positionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    //glBufferData(GL_ARRAY_BUFFER, numPoints * sizeof(float), positions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribDivisor(1, 1);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &colorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    //glBufferData(GL_ARRAY_BUFFER, numPoints * sizeof(float), colors.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribDivisor(2, 1);
    glEnableVertexAttribArray(2);

    shader = new QOpenGLShaderProgram();
    shader->addShaderFromSourceCode(QOpenGLShader::Vertex, plotVertexSource);
    shader->addShaderFromSourceCode(QOpenGLShader::Fragment, plotFragmentSource);
    shader->link();

    selectionShader = new QOpenGLShaderProgram();
    selectionShader->addShaderFromSourceCode(QOpenGLShader::Vertex, selectionVertexSource);
    selectionShader->addShaderFromSourceCode(QOpenGLShader::Fragment, selectionFragmentSource);
    shader->link();
}

void ScatterplotWidget::resizeGL(int w, int h)
{
    _windowSize.setWidth(w);
    _windowSize.setHeight(h);
}

void ScatterplotWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    qDebug() << "Rendering scatterplot";
    QPointF ns(selectionStart.x() < selectionEnd.x() ? selectionStart.x() : selectionEnd.x(), selectionStart.y() < selectionEnd.y() ? selectionStart.y() : selectionEnd.y());
    QPointF ne(selectionStart.x() < selectionEnd.x() ? selectionEnd.x() : selectionStart.x(), selectionStart.y() < selectionEnd.y() ? selectionEnd.y() : selectionStart.y());

    QRectF selection(ns, ne);

    shader->bind();
    if (_scalingMode == Relative)
    {
        shader->setUniformValue("pointSize", _pointSize / 800);
    }
    else if (_scalingMode == Absolute) {
        shader->setUniformValue("pointSize", _pointSize / _windowSize.width());
    }
    
    shader->setUniformValue("alpha", _alpha);
    shader->setUniformValue("start", selection.topLeft());
    shader->setUniformValue("end", selection.bottomRight());
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, numPoints);

    // Selection
    selectionShader->bind();

    //qDebug() << ns << ne;
    selectionShader->setUniformValue("start", selection.topLeft());
    selectionShader->setUniformValue("end", selection.bottomRight());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void ScatterplotWidget::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "Mouse clicky";
    _selecting = true;

    selectionStart = QPointF((float)event->x() / _windowSize.width(), 1 - ((float)event->y() / _windowSize.height()));
    selectionStart = selectionStart * 2 - QPointF(1, 1);
}

void ScatterplotWidget::mouseMoveEvent(QMouseEvent *event)
{
    //qDebug() << "Mouse movey";
    if (_selecting) {
        selectionEnd = QPointF((float)event->x() / _windowSize.width(), 1 - ((float)event->y() / _windowSize.height()));
        selectionEnd = selectionEnd * 2 - QPointF(1, 1);
        update();
    }
}

void ScatterplotWidget::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug() << "Mouse releasey";
    _selecting = false;

    selectionEnd = QPointF((float)event->x() / _windowSize.width(), 1 - ((float)event->y() / _windowSize.height()));
    selectionEnd = selectionEnd * 2 - QPointF(1, 1);

    QRectF selection(selectionStart, selectionEnd);
    onSelection(selection);
}

void ScatterplotWidget::onSelection(QRectF selection)
{
    update();

    std::vector<unsigned int> indices;
    for (int i = 0; i < numPoints; i++)
    {
        QPointF point((*positions)[i * 2 + 0], (*positions)[i * 2 + 1]);

        if (selection.contains(point))
        {
            indices.push_back(i);
        }
    }

    for (const plugin::SelectionListener* listener : selectionListeners)
    {
        listener->onSelection(indices);
    }
}

} // namespace gui

} // namespace hdps
