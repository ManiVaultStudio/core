#include "DensityPlotWidget.h"

#include <vector>
#include <algorithm>
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

    qDebug() << "Setting position data";
    meanShift.setData(positions);

    _needsDensityMapUpdate = true;
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

    glClearColor(1, 1, 1, 1);
    qDebug() << "Initializing density plot";

    meanShift.init();

    bool loaded = _shaderDensityDraw.loadShaderFromFile(":shaders/Quad.vert", ":shaders/DensityDraw.frag");
    if (!loaded) {
        qDebug() << "Failed to load DensityDraw shader";
    }

    loaded = _shaderGradientDraw.loadShaderFromFile(":shaders/Quad.vert", ":shaders/GradientDraw.frag");
    if (!loaded) {
        qDebug() << "Failed to load GradientDraw shader";
    }

    loaded = _shaderMeanShiftDraw.loadShaderFromFile(":shaders/Quad.vert", ":shaders/Texture.frag");
    if (!loaded) {
        qDebug() << "Failed to load MeanShiftDraw shader";
    }
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
    qDebug() << "Drawing density plot";

    if (_needsDensityMapUpdate)
    {
        meanShift.computeDensity();
        _needsDensityMapUpdate = false;
    }
    
    meanShift.computeGradient();
    meanShift.computeMeanShift();

    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
    glViewport(0, 0, _windowSize.width(), _windowSize.height());

    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //drawDensity();
    //drawGradient();
    drawMeanShift();
}

void DensityPlotWidget::drawDensity()
{
    if (_numPoints == 0) return;

    _shaderDensityDraw.bind();

    meanShift.getDensityTexture().bind(0);
    _shaderDensityDraw.uniform1i("tex", 0);
    _shaderDensityDraw.uniform1f("norm", 1 / meanShift._maxKDE);

    meanShift.drawFullscreenQuad();
}

void DensityPlotWidget::drawGradient()
{
    if (_numPoints == 0) return;

    _shaderGradientDraw.bind();

    meanShift.getGradientTexture().bind(0);
    _shaderGradientDraw.uniform1i("tex", 0);

    meanShift.drawFullscreenQuad();
}

void DensityPlotWidget::drawMeanShift()
{
    if (_numPoints == 0) return;

    _shaderMeanShiftDraw.bind();

    meanShift.getMeanShiftTexture().bind(0);
    _shaderMeanShiftDraw.uniform1i("tex", 0);

    meanShift.drawFullscreenQuad();
}


//void DensityPlotWidget::createSampleSelectionTextureBuffer()
//{
//    if (_activeSampleTexture == 0)
//        glGenTextures(1, &_activeSampleTexture);
//
//    glBindTexture(GL_TEXTURE_1D, _activeSampleTexture);
//
//    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//
//    //glTexImage1D(GL_TEXTURE_1D, 0, GL_R32I, _localSampleSelectionForBuffer.size(), 0, GL_RED_INTEGER, GL_INT, _localSampleSelectionForBuffer.data());
//
//    glBindTexture(GL_TEXTURE_1D, 0);
//}


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

    //onSelection(_selection);
}

//void DensityPlotWidget::onSelection(Selection selection)
//{
//    update();
//
//    std::vector<unsigned int> indices;
//    for (unsigned int i = 0; i < _numPoints; i++)
//    {
//        Vector2f point = (*_positions)[i];
//        point.x *= _windowSize.width() / _windowSize.height();
//
//        if (selection.contains(point))
//            indices.push_back(i);
//    }
//
//    for (const plugin::SelectionListener* listener : _selectionListeners)
//        listener->onSelection(indices);
//}

void DensityPlotWidget::cleanup()
{
    qDebug() << "Deleting density plot widget, performing clean up...";
    makeCurrent();

    // Delete objects
    meanShift.cleanup();
}

} // namespace gui

} // namespace hdps
