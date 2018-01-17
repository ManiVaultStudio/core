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
    DensityPlotWidget() : _sigma(0.15f), _msTexSize(512) { }

    void setData(const std::vector<Vector2f>* data);
    void setColors(const std::vector<Vector3f>& data);
    void addSelectionListener(const plugin::SelectionListener* listener);

    void createGaussianTexture();
protected:
    void initializeGL()         Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    void paintGL()              Q_DECL_OVERRIDE;
    void drawDensityOffscreen();
    void drawDensity();
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

    const float        DEFAULT_POINT_SIZE      = 15;
    const Vector3f     DEFAULT_SELECTION_COLOR = Vector3f(1.0f, 0.5f, 0.0f);
    const float        DEFAULT_ALPHA_VALUE     = 0.5f;

    GLuint _vao;

    BufferObject _positionBuffer;
    BufferObject _colorBuffer;

    ShaderProgram _shaderDensitySplat;
    ShaderProgram _shaderDensityDraw;

    unsigned int _numPoints = 0;
    const std::vector<Vector2f>* _positions;
    std::vector<Vector3f> _colors;

    QSize _windowSize;
    Vector3f _selectionColor  = DEFAULT_SELECTION_COLOR;
    float _pointSize          = DEFAULT_POINT_SIZE;
    float _alpha              = DEFAULT_ALPHA_VALUE;

    //std::unique_ptr<QOpenGLShaderProgram> _selectionShader;
    bool _selecting = false;
    Selection _selection;
    std::vector<const plugin::SelectionListener*> _selectionListeners;


    /////////////////
    GaussianTexture _gaussTexture;
    Framebuffer _pdfFBO;
    Texture2D _pdfTexture;

    float _sigma;
    GLuint _activeSampleTexture;

    float _maxKDE;
    size_t _msTexSize;
};

} // namespace gui

} // namespace hdps

#endif // DENSITY_PLOT_WIDGET_H
