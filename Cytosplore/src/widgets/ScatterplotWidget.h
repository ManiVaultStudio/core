#ifndef SCATTERPLOT_WIDGET_H
#define SCATTERPLOT_WIDGET_H

#include "../SelectionListener.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>

#include <QMouseEvent>
#include <QPoint>
#include <QRectF>

enum PointScaling {
    Absolute, Relative
};

namespace hdps
{
namespace gui
{

class ScatterplotWidget : public QOpenGLWidget, QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    void setData(const std::vector<float>* data);
    void setColors(const std::vector<float>& data);
    void setPointSize(const float size);
    void setAlpha(const float alpha);
    void setPointScaling(PointScaling scalingMode);
    void addSelectionListener(const plugin::SelectionListener* listener);
protected:
    void initializeGL()         Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    void paintGL()              Q_DECL_OVERRIDE;

    void mousePressEvent(QMouseEvent *event)   Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event)    Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

    void onSelection(QRectF selection);

private:
    const float        DEFAULT_POINT_SIZE    = 15;
    const float        DEFAULT_ALPHA_VALUE   = 0.5f;
    const PointScaling DEFAULT_POINT_SCALING = PointScaling::Relative;

    GLuint vao;
    GLuint positionBuffer;
    GLuint colorBuffer;
    QOpenGLShaderProgram* shader;

    int numPoints = 0;
    const std::vector<float>* positions;
    std::vector<float> colors;

    QSize _windowSize;
    PointScaling _scalingMode = DEFAULT_POINT_SCALING;
    float _pointSize          = DEFAULT_POINT_SIZE;
    float _alpha              = DEFAULT_ALPHA_VALUE;

    QOpenGLShaderProgram* selectionShader;
    bool _selecting = false;
    QPoint selectionStart;
    QPoint selectionEnd;
    std::vector<const plugin::SelectionListener*> selectionListeners;
};

} // namespace gui

} // namespace hdps

#endif // SCATTERPLOT_WIDGET_H
