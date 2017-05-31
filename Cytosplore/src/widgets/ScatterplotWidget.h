#ifndef SCATTERPLOT_WIDGET_H
#define SCATTERPLOT_WIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>

enum PointScaling {
    Absolute, Relative
};

class ScatterplotWidget : public QOpenGLWidget, QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    void setData(const std::vector<float>& data);
    void setColors(const std::vector<float>& data);
    void setPointSize(const float size);
    void setAlpha(const float alpha);
    void setPointScaling(PointScaling scalingMode);
protected:
    void initializeGL()         Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    void paintGL()              Q_DECL_OVERRIDE;

private:
    const float        DEFAULT_POINT_SIZE    = 15;
    const float        DEFAULT_ALPHA_VALUE   = 0.5f;
    const PointScaling DEFAULT_POINT_SCALING = PointScaling::Relative;

    GLuint vao;
    GLuint positionBuffer;
    GLuint colorBuffer;
    QOpenGLShaderProgram* shader;

    std::vector<float> positions;
    std::vector<float> colors;

    QSize _windowSize;
    PointScaling _scalingMode = DEFAULT_POINT_SCALING;
    float _pointSize          = DEFAULT_POINT_SIZE;
    float _alpha              = DEFAULT_ALPHA_VALUE;
};

#endif // SCATTERPLOT_WIDGET_H
