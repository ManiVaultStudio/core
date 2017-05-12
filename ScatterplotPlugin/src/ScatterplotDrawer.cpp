#include "ScatterplotDrawer.h"

#include <QDebug>

#include <vector>
#include <cstdlib>

#define GLSL(version, shader)  "#version " #version "\n" #shader

void ScatterplotDrawer::setData(const std::vector<float>& positions) {
    this->positions = positions;

    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * 2 * sizeof(float), positions.data(), GL_STATIC_DRAW);
}

void ScatterplotDrawer::setPointSize(const float size) {
    _pointSize = size;
}

void ScatterplotDrawer::initializeGL()
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
    glBufferData(GL_ARRAY_BUFFER, positions.size() * 2 * sizeof(float), positions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribDivisor(1, 1);
    glEnableVertexAttribArray(1);

    const char *vertexSource = GLSL(330,
        uniform vec2 windowSize;
        uniform float pointSize;

        in vec2 vertex;
        in vec2 position;

        out vec2 pass_texCoords;

        void main()
        {
            pass_texCoords = vertex;
            gl_Position = vec4(vertex * pointSize/windowSize + position, 0, 1);
        }
    );

    const char *fragmentSource = GLSL(330,
        uniform vec2 windowSize;
        uniform float pointSize;

        in vec2 pass_texCoords;

        out vec4 fragColor;

        void main()
        {
            float len = length(pass_texCoords);
            // If the fragment is outside of the circle discard it
            if (len > 1)
                discard;

            float edge = fwidth(len);
            float a = smoothstep(1, 1 - edge, len);
            fragColor = vec4(0, 0.5, 1.0, a / 2.0);

            //// Determine alpha based on distance from center
            //vec2 pointPix = windowSize * pointSize;
            //vec2 pixel = 1.0 / vec2(pointPix.x, pointPix.y);
            //float scale = windowSize.x / 128;
            //float a = smoothstep(1, 1 - pixel.x*scale, len);
            //fragColor = vec4(0, 0.5, 1.0, a / 2.0);

        }
    );

    shader = new QOpenGLShaderProgram();
    shader->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexSource);
    shader->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentSource);
    shader->link();
}

void ScatterplotDrawer::resizeGL(int w, int h)
{
    shader->bind();
    shader->setUniformValue("windowSize", w, h);
    shader->release();
}

void ScatterplotDrawer::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    qDebug() << "Rendering scatterplot";

    shader->bind();
    shader->setUniformValue("pointSize", _pointSize);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, positions.size());
}
