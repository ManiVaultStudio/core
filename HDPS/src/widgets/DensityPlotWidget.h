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
    DensityPlotWidget() : _needsDensityMapUpdate(true) {}

    void setData(const std::vector<Vector2f>* data);
    void addSelectionListener(const plugin::SelectionListener* listener);

protected:
    void initializeGL()         Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    void paintGL()              Q_DECL_OVERRIDE;

    void drawDensity();
    void drawGradient();
    void drawMeanShift();

    //void createSampleSelectionTextureBuffer();

    void mousePressEvent(QMouseEvent *event)   Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event)    Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

    //void onSelection(Selection selection);
    void cleanup();

private:
    const Matrix3f toClipCoordinates = Matrix3f(2, 0, 0, 2, -1, -1);
    Matrix3f toNormalisedCoordinates;
    Matrix3f toIsotropicCoordinates;

    QSize _windowSize;

    bool _selecting = false;
    Selection _selection;
    std::vector<const plugin::SelectionListener*> _selectionListeners;

    //GLuint _activeSampleTexture;

    unsigned int _numPoints = 0;

    ShaderProgram _shaderDensityDraw;
    ShaderProgram _shaderGradientDraw;
    ShaderProgram _shaderMeanShiftDraw;
    MeanShift meanShift;

    bool _needsDensityMapUpdate;
};

} // namespace gui

} // namespace hdps

#endif // DENSITY_PLOT_WIDGET_H
