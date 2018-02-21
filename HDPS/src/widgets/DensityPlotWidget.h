#ifndef DENSITY_PLOT_WIDGET_H
#define DENSITY_PLOT_WIDGET_H

#include "../SelectionListener.h"

#include "../graphics/BufferObject.h"
#include "../graphics/Framebuffer.h"
#include "../graphics/Vector2f.h"
#include "../graphics/Vector3f.h"
#include "../graphics/Matrix3f.h"
#include "../graphics/Selection.h"
#include "../graphics/Texture.h"
#include "../graphics/Shader.h"

#include "../util/MeanShift.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>

#include <QMouseEvent>
#include <memory>

namespace hdps
{
namespace gui
{

class DensityPlotWidget : public QOpenGLWidget, QOpenGLFunctions_3_3_Core
{
    Q_OBJECT

public:
    enum RenderMode {
        DENSITY, GRADIENT, PARTITIONS
    };

    DensityPlotWidget(RenderMode renderMode) : _renderMode(renderMode) {}

    void setRenderMode(RenderMode renderMode);
    void setData(const std::vector<Vector2f>* data);
    void addSelectionListener(const plugin::SelectionListener* listener);

protected:
    void initializeGL()         Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    void paintGL()              Q_DECL_OVERRIDE;

    void drawDensity();
    void drawGradient();
    void drawMeanShift();
    void drawLandscape();

    void cleanup();

private:
    QSize _windowSize;

    unsigned int _numPoints = 0;

    ShaderProgram _shaderDensityDraw;
    ShaderProgram _shaderGradientDraw;
    ShaderProgram _shaderMeanShiftDraw;
    MeanShift _meanShift;

    RenderMode _renderMode;
};

} // namespace gui

} // namespace hdps

#endif // DENSITY_PLOT_WIDGET_H
