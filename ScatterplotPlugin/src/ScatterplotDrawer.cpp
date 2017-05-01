#include "ScatterplotDrawer.h"

#include <QOpenGLFunctions>
#include <QDebug>

#include "ShaderLoader.h"

void ScatterplotDrawer::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(1.0, 0.0, 0.0, 1.0);
    qDebug() << "Initializing scatterplot";

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    float vertices[6*2] = 
    {
        -0.5, -0.5,
         0.5, -0.5,
        -0.5,  0.5,
        -0.5,  0.5,
         0.5, -0.5,
         0.5,  0.5
    };

    float texCoords[6*2] =
    {
        -1, -1,
         1, -1,
        -1,  1,
        -1,  1,
         1, -1,
         1,  1
    };

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    GLuint tbo;
    glGenBuffers(1, &tbo);
    glBindBuffer(GL_ARRAY_BUFFER, tbo);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), texCoords, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    shader = ShaderLoader::loadShaderProgram();
}

void ScatterplotDrawer::resizeGL(int w, int h)
{

}

void ScatterplotDrawer::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    qDebug() << "Rendering scatterplot";

    glUseProgram(shader);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
