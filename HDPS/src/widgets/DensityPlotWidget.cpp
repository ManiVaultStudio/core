#include "DensityPlotWidget.h"

#include <vector>

#include <QDebug>

#define GLSL(version, shader)  "#version " #version "\n" #shader

namespace hdps
{
namespace gui
{

// Positions need to be passed as a pointer as we need to store them locally in order
// to be able to find the subset of data that's part of a selection. If passed
// by reference then we can upload the data to the GPU, but not store it in the widget.
    void DensityPlotWidget::setData(const std::vector<Vector2f>* positions)
{
    _numPoints = (unsigned int)positions->size();
    _positions = positions;
    //_colors.clear();
    //_colors.resize(_numPoints, Vector3f(0.5f, 0.5f, 0.5f));
    qDebug() << "Setting position data";
    _positionBuffer.bind();
    _positionBuffer.setData(*positions);
    //_colorBuffer.bind();
    //_colorBuffer.setData(_colors);
    qDebug() << "Updating";
    update();
}

void GaussianTexture::generate()
{
    int w = 32;
    int h = 32;

    float* data = new float[w * h];

    double sigma = w / 6.0;
    double i_c = w / 2.0;
    double j_c = h / 2.0;

    for (int j = 0; j < h; j++){
        for (int i = 0; i < w; i++){

            const double sqrt_dist = (i - i_c)*(i - i_c) + (j - j_c)*(j - j_c);
            const double val = exp(sqrt_dist / (-2.0 * sigma * sigma)) / (2.0 * 3.1415926535 * sigma * sigma);

            data[j*w + i] = val*1000.0;// texture_multiplier;
        }
    }

    glBindTexture(GL_TEXTURE_2D, _handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, w, h, 0, GL_RED, GL_FLOAT, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    delete[] data;
}

void DensityPlotWidget::setColors(const std::vector<Vector3f>& colors)
{
    _colors = colors;

    _colorBuffer.bind();
    _colorBuffer.setData(colors);
    update();
}

void DensityPlotWidget::addSelectionListener(const plugin::SelectionListener* listener)
{
    _selectionListeners.push_back(listener);
}

void DensityPlotWidget::initializeGL()
{
    initializeOpenGLFunctions();

    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &DensityPlotWidget::cleanup);

    glClearColor(0.0, 0.0, 1.0, 1.0);
    qDebug() << "Initializing density plot";

    _gaussTexture = new GaussianTexture();
    _gaussTexture->generate();

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

    std::vector<float> texCoords(
    {
        0, 0,
        1, 0,
        0, 1,
        0, 1,
        1, 0,
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

    BufferObject texQuad;
    texQuad.create();
    texQuad.bind();
    texQuad.setData(texCoords);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    _positionBuffer.create();
    _positionBuffer.bind();
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribDivisor(2, 1);
    glEnableVertexAttribArray(2);

    //_colorBuffer.create();
    //_colorBuffer.bind();
    //glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //glVertexAttribDivisor(2, 1);
    //glEnableVertexAttribArray(2);

    if (_numPoints > 0)
    {
        _positionBuffer.bind();
        _positionBuffer.setData(*_positions);
        //_colorBuffer.bind();
        //_colorBuffer.setData(_colors);
    }

    bool loaded = _offscreenDensityShader.loadShaderFromFile(":shaders/SimpleShader.vert", ":shaders/SimpleShader.frag");
    if (!loaded) {
        qDebug() << "Failed to load SimpleShader";
    }
    //_shader = std::make_unique<QOpenGLShaderProgram>();
    //_shader->addShaderFromSourceCode(QOpenGLShader::Vertex, plotVertexSource);
    //_shader->addShaderFromSourceCode(QOpenGLShader::Fragment, plotFragmentSource);
    //_shader->link();

    //_selectionShader = std::make_unique<QOpenGLShaderProgram>();
    //_selectionShader->addShaderFromSourceCode(QOpenGLShader::Vertex, selectionVertexSource);
    //_selectionShader->addShaderFromSourceCode(QOpenGLShader::Fragment, selectionFragmentSource);
    //_shader->link();
}

void DensityPlotWidget::resizeGL(int w, int h)
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

void DensityPlotWidget::paintGL()
{
    //qDebug() << "Rendering scatterplot";
    //glClear(GL_COLOR_BUFFER_BIT);

    //int w = _windowSize.width();
    //int h = _windowSize.height();
    //int size = w < h ? w : h;
    //glViewport(w / 2 - size / 2, h / 2 - size / 2, size, size);

    //Vector2f topLeft = toClipCoordinates * toIsotropicCoordinates * _selection.topLeft();
    //Vector2f bottomRight = toClipCoordinates * toIsotropicCoordinates * _selection.bottomRight();

    //_shader->bind();

    //_shader->setUniformValue("pointSize", _pointSize / _windowSize.width());

    //_shader->setUniformValue("selectionColor", _selectionColor.x, _selectionColor.y, _selectionColor.z);
    //_shader->setUniformValue("alpha", _alpha);
    //_shader->setUniformValue("selecting", _selecting);
    //_shader->setUniformValue("start", topLeft.x, topLeft.y);
    //_shader->setUniformValue("end", bottomRight.x, bottomRight.y);
    //glDrawArraysInstanced(GL_TRIANGLES, 0, 6, _numPoints);

    //if (_selecting)
    //{
    //    topLeft = toClipCoordinates * _selection.topLeft();
    //    bottomRight = toClipCoordinates * _selection.bottomRight();

    //    glViewport(0, 0, w, h);

    //    // Selection
    //    _selectionShader->bind();
    //    _selectionShader->setUniformValue("start", topLeft.x, topLeft.y);
    //    _selectionShader->setUniformValue("end", bottomRight.x, bottomRight.y);
    //    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //}
    drawDensityOffscreen();
}

void DensityPlotWidget::drawDensityOffscreen()
{
    //std::cout << "drawing density with " << _numPoints << " vertices.\n";

    //std::cout << "density offscreen pass\n";

    //GLint fb;
    //glGetIntegerv(GL_FRAMEBUFFER_BINDING, &fb);
    //std::cout << "	Binding Offscreen FBO = " << _pdfFBO << ".\n";
    //GLuint fbo;
    //glGenFramebuffers(1, &fbo);
    //glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    //glDrawBuffer(GL_COLOR_ATTACHMENT0);
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //TEMP
    GLsizei _msTexSize = 512;
    // <Q> Whats _msTexSize?
    glViewport(0, 0, _msTexSize, _msTexSize);

    int w = _windowSize.width();
    int h = _windowSize.height();
    int size = w < h ? w : h;
    glViewport(w / 2 - size / 2, h / 2 - size / 2, size, size);

    // Set background color
    //glClearColor(1, 0, 0, 1);
    // Clear fbo
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Enable additive blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    // Bind shader
    _offscreenDensityShader.bind();

    // Set mvp uniform
    //_offscreenDensityShader.uniformMatrix4f("modelViewProjectionMatrix", _mvp);

    // Set sigma uniform
    //>>>>_offscreenDensityShader.uniform1f("sigma", _sigma);

    // Set advanced parameters uniform
    //>>>> float params[4] = { (_isScaleAvailable ? 1.0f / _maxScale : -1.0f), 0.0f, 0.0f, 0.0f };
    //>>>> _offscreenDensityShader.setParameter4fv(advancedParamsUniform, params);

    // Set gauss texture
    _gaussTexture->bind(0);
    _offscreenDensityShader.uniform1i("gaussSampler", 0);

    // Set the texture containing flags for the active state of each sample 
    //>>>> _offscreenDensityShader.uniform1i("activeSampleSampler", 1);
    //>>>> glActiveTexture(GL_TEXTURE1);
    //>>>> glBindTexture(GL_TEXTURE_1D, _activeSampleTexture);

    // Bind the vao
    glBindVertexArray(_vao);

    // Bind file ID buffer
    //if (_isFileIdxAvailable)
    //{
    //    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferSample);
    //    glVertexAttribIPointer(sampleIdAttribute, 1, GL_INT, 0, 0);
    //    glEnableVertexAttribArray(sampleIdAttribute);
    //}
    qDebug() << _numPoints;
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, _numPoints);
    //glDrawArrays(GL_TRIANGLES, 0, 6);

    // Unbind vao
    glBindVertexArray(0);

    // Unbind shader
    _offscreenDensityShader.release();

    // Read pixels from framebuffer
    //std::vector<float> kde(_msTexSize * _msTexSize * 3);

    //glReadBuffer(GL_COLOR_ATTACHMENT0);
    //glReadPixels(0, 0, _msTexSize, _msTexSize, GL_RGB, GL_FLOAT, kde.data());

    // Calculate max value for normalization
    //>>>> _maxKDE = -99999999.9f;
    //>>>> for (int i = 0; i < kde.size(); i += 3) // only lookup red channel
    //>>>> {
    //>>>>     _maxKDE = std::max(_maxKDE, kde[i]);
    //>>>> }




    //std::cout << "	Max KDE Value = " << _maxKDE << ".\n";

    //QImage pluto(_msTexSize, _msTexSize, QImage::Format::Format_ARGB32);
    //for (int j = 0; j < _msTexSize; ++j)
    //{
    //	for (int i = 0; i < _msTexSize; ++i)
    //	{
    //		int idx = j * _msTexSize + i;
    //		pluto.setPixel(i, j, qRgb(pippo[idx * 3] * 255, pippo[idx * 3 + 1] * 255, pippo[idx * 3 + 2] * 255));
    //	}
    //}
    //pluto.save("pluto.png");

    //std::cout << "	Binding Onscreen FBO = " << _defaultFBO << ".\n";

    // Bind the default fbo
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Reset blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void DensityPlotWidget::createSampleSelectionTextureBuffer()
{
    if (_activeSampleTexture == 0)
        glGenTextures(1, &_activeSampleTexture);

    glBindTexture(GL_TEXTURE_1D, _activeSampleTexture);

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    //glTexImage1D(GL_TEXTURE_1D, 0, GL_R32I, _localSampleSelectionForBuffer.size(), 0, GL_RED_INTEGER, GL_INT, _localSampleSelectionForBuffer.data());

    glBindTexture(GL_TEXTURE_1D, 0);
}

void DensityPlotWidget::mousePressEvent(QMouseEvent *event)
{
    _selecting = true;

    Vector2f point = toNormalisedCoordinates * Vector2f(event->x(), _windowSize.height() - event->y());
    _selection.setStart(point);
}

void DensityPlotWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!_selecting) return;

    Vector2f point = toNormalisedCoordinates * Vector2f(event->x(), _windowSize.height() - event->y());
    _selection.setEnd(point);

    update();
}

void DensityPlotWidget::mouseReleaseEvent(QMouseEvent *event)
{
    _selecting = false;

    Vector2f point = toNormalisedCoordinates * Vector2f(event->x(), _windowSize.height() - event->y());
    _selection.setEnd(point);

    onSelection(_selection);
}

void DensityPlotWidget::onSelection(Selection selection)
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

void DensityPlotWidget::cleanup()
{
    qDebug() << "Deleting scatterplot widget, performing clean up...";
    makeCurrent();

    delete _gaussTexture;

    glDeleteVertexArrays(1, &_vao);
    _positionBuffer.destroy();
    //_colorBuffer.destroy();
}

} // namespace gui

} // namespace hdps
