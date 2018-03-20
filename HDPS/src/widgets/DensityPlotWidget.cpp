#include "DensityPlotWidget.h"

#include <vector>
#include <algorithm>
#include <QDebug>

namespace hdps
{
namespace gui
{

DensityPlotWidget::DensityPlotWidget()
    :
    _densityRenderer(DensityRenderer::DENSITY)
{
    
}

DensityPlotWidget::~DensityPlotWidget()
{
    // Delete objects
    _densityRenderer.terminate();
}

// Points need to be passed as a pointer as we need to store them locally in order
// to be able to recompute the densities when parameters change.
void DensityPlotWidget::setData(const std::vector<Vector2f>* points)
{
    _densityRenderer.setData(points);

    update();
}

void DensityPlotWidget::setSigma(const float sigma)
{
    _densityRenderer.setSigma(sigma);

    update();
}

void DensityPlotWidget::initializeGL()
{
    qDebug() << "Initializing density plot GL";
    initializeOpenGLFunctions();

    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &DensityPlotWidget::terminateGL);

    _densityRenderer.init(context());
    qDebug() << "Initialized density plot GL";
}

void DensityPlotWidget::resizeGL(int w, int h)
{
    _densityRenderer.resize(w, h);
}

void DensityPlotWidget::paintGL()
{
    qDebug() << "Rendering densityplot";

    // Bind the framebuffer belonging to the widget
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
    
    _densityRenderer.render();
    qDebug() << "Done rendering densityplot";
}

void DensityPlotWidget::terminateGL()
{
    qDebug() << "Deleting density plot widget, performing clean up...";
    makeCurrent();

    _densityRenderer.terminate();
}

} // namespace gui

} // namespace hdps
