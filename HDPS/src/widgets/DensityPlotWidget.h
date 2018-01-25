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

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>

#include <QMouseEvent>
#include <memory>

namespace hdps
{
namespace gui
{

class GaussianTexture : public Texture2D
{
public:
    void generate();
};

class DensityPlotWidget : public QOpenGLWidget, QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    DensityPlotWidget() : _sigma(0.15f), _msTexSize(512), _needsDensityMapUpdate(true) {}

    void setData(const std::vector<Vector2f>* data);
    void addSelectionListener(const plugin::SelectionListener* listener);

    void createGaussianTexture();
protected:
    void initializeGL()         Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    void paintGL()              Q_DECL_OVERRIDE;

    void drawFullscreenQuad();
    void drawDensityOffscreen();
    void drawGradientOffscreen();
    void drawMeanshiftOffscreen();
    void drawDensity();
    void drawGradient();
    void createSampleSelectionTextureBuffer();

    void mousePressEvent(QMouseEvent *event)   Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event)    Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

    void onSelection(Selection selection);
    void cleanup();

private:
    const Matrix3f toClipCoordinates = Matrix3f(2, 0, 0, 2, -1, -1);
    Matrix3f toNormalisedCoordinates;
    Matrix3f toIsotropicCoordinates;

    GLuint _vao;
    GLuint _quad;

    BufferObject _positionBuffer;

    ShaderProgram _shaderDensityCompute;
    ShaderProgram _shaderDensityDraw;
    ShaderProgram _shaderGradientCompute;
    ShaderProgram _shaderGradientDraw;
    ShaderProgram _shaderMeanshiftCompute;

    unsigned int _numPoints = 0;
    const std::vector<Vector2f>* _positions;

    QSize _windowSize;

    bool _selecting = false;
    Selection _selection;
    std::vector<const plugin::SelectionListener*> _selectionListeners;


    /////////////////
    GaussianTexture _gaussTexture;
    Framebuffer _meanshiftFramebuffer;
    Texture2D _densityTexture;
    Texture2D _gradientTexture;
    Texture2D _meanshiftTexture;

    float _sigma;
    GLuint _activeSampleTexture;

    float _maxKDE;
    size_t _msTexSize;

    bool _needsDensityMapUpdate;
    std::vector<float> _meanShiftMapCPU;
};

} // namespace gui

} // namespace hdps

#endif // DENSITY_PLOT_WIDGET_H
