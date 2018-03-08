#include "DensityPlotWidget.h"

#include <vector>
#include <algorithm>
#include <QDebug>

#define GLSL(version, shader)  "#version " #version "\n" #shader

namespace hdps
{
namespace gui
{

DensityPlotWidget::DensityPlotWidget(RenderMode renderMode)
    :
    _renderMode(renderMode)
{
    
}

DensityPlotWidget::~DensityPlotWidget()
{
    // Delete objects
    _densityComputation.cleanup();
}

void DensityPlotWidget::setRenderMode(RenderMode renderMode)
{
    _renderMode = renderMode;

    update();
}

// Points need to be passed as a pointer as we need to store them locally in order
// to be able to recompute the densities when parameters change.
void DensityPlotWidget::setData(const std::vector<Vector2f>* points)
{
    _densityComputation.setData(points);

    update();
}

void DensityPlotWidget::setSigma(const float sigma)
{
    _densityComputation.setSigma(sigma);

    update();
}

void DensityPlotWidget::initializeGL()
{
    qDebug() << "Initializing density plot GL";
    initializeOpenGLFunctions();

    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &DensityPlotWidget::terminateGL);

    // Create a simple VAO for full-screen quad rendering
    glGenVertexArrays(1, &_quad);

    // Load the necessary shaders for density drawing
    bool loaded = true;
    loaded &= _shaderDensityDraw.loadShaderFromFile(":shaders/Quad.vert", ":shaders/DensityDraw.frag");
    loaded &= _shaderIsoDensityDraw.loadShaderFromFile(":shaders/Quad.vert", ":shaders/IsoDensityDraw.frag");
    if (!loaded) {
        qDebug() << "Failed to load one of the Density shaders";
    }

    // Load the color map
    _colorMap.loadFromFile(":colormaps/Spectral.png");

    // Initialize the density computation
    _densityComputation.init(context());
    // Compute the density in case data was already set
    _densityComputation.compute();
    qDebug() << "Initialized density plot GL";
}

void DensityPlotWidget::resizeGL(int w, int h)
{
    _windowSize.setWidth(w);
    _windowSize.setHeight(h);
}

void DensityPlotWidget::paintGL()
{
    std::vector<std::vector<unsigned int>> clusters;

    _meanShift.cluster(clusters);

    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
    glViewport(0, 0, _windowSize.width(), _windowSize.height());

    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    switch (_renderMode) {
        case DENSITY: drawDensity(); break;
        case GRADIENT: drawGradient(); break;
        case PARTITIONS: drawMeanShift(); break;
        case LANDSCAPE: drawLandscape(); break;
    }
}

void DensityPlotWidget::terminateGL()
void DensityPlotWidget::drawDensity()
{
    qDebug() << "Deleting density plot widget, performing clean up...";
    makeCurrent();

    _shaderDensityDraw.destroy();
    _shaderIsoDensityDraw.destroy();
    _densityComputation.cleanup();
    _colorMap.destroy();

    glDeleteVertexArrays(1, &_quad);
}

void DensityPlotWidget::drawGradient()
{
    if (_numPoints == 0) return;

    _shaderGradientDraw.bind();

    _meanShift.getGradientTexture().bind(0);
    _shaderGradientDraw.uniform1i("tex", 0);

    _meanShift.drawFullscreenQuad();
}

void DensityPlotWidget::drawMeanShift()
{
    if (_numPoints == 0) return;

    _shaderMeanShiftDraw.bind();

    _meanShift.getMeanShiftTexture().bind(0);
    _shaderMeanShiftDraw.uniform1i("tex", 0);

    _meanShift.drawFullscreenQuad();
}

void DensityPlotWidget::drawLandscape()
{
    if (_numPoints == 0) return;

    _shaderIsoDensityDraw.bind();

    _meanShift.getDensityTexture().bind(0);
    _shaderIsoDensityDraw.uniform1i("tex", 0);

    _shaderIsoDensityDraw.uniform4f("renderParams", 1.0f / _meanShift.getMaxDensity(), 0, 1.0f / _numPoints, 0);

    colorMap.bind(1);
    _shaderIsoDensityDraw.uniform1i("colorMap", 1);

    _meanShift.drawFullscreenQuad();
}

void DensityPlotWidget::cleanup()
{
    qDebug() << "Deleting density plot widget, performing clean up...";
    makeCurrent();

    // Delete objects
    _meanShift.cleanup();
}

} // namespace gui

} // namespace hdps
