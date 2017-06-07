#include "ImageWidget.h"

#include <vector>

#include <QDebug>

#define GLSL(version, shader)  "#version " #version "\n" #shader

void ImageWidget::setImage(const std::vector<float>& pixels, int width, int height)
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_FLOAT, pixels.data());

    update();
}

void ImageWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(1.0, 1.0, 1.0, 1.0);
    qDebug() << "Initializing image widget";

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    float vertices[6 * 2] =
    {
        -1, -1,
        1, -1,
        -1, 1,
        -1, 1,
        1, -1,
        1, 1
    };

    float texCoords[6 * 2] =
    {
        0, 0,
        1, 0,
        0, 1,
        0, 1,
        1, 0,
        1, 1
    };

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

    const char *vertexSource = GLSL(330,
        in vec2 position;
        in vec2 texCoords;

        out vec2 pass_texCoords;

        void main()
        {
            pass_texCoords = texCoords;
            gl_Position = vec4(position, 0, 1);
        }
    );

    const char *fragmentSource = GLSL(330,
        uniform sampler2D image;

        in vec2 pass_texCoords;

        out vec4 fragColor;

        void main()
        {
            fragColor = texture(image, pass_texCoords);
        }
    );

    shader = new QOpenGLShaderProgram();
    shader->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexSource);
    shader->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentSource);
    shader->link();
}

void ImageWidget::resizeGL(int w, int h)
{
    _windowSize.setWidth(w);
    _windowSize.setHeight(h);
}

void ImageWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    qDebug() << "Rendering scatterplot";

    shader->bind();
    //shader->setUniformValue("pointSize", _pointSize / 800);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    shader->setUniformValue("image", 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}
