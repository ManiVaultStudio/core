#ifndef SCATTERPLOT_DRAWER_H
#define SCATTERPLOT_DRAWER_H

#include <QObject>
#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>

class ScatterplotDrawer : public QOpenGLWidget, QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    void setData(const std::vector<float>& data);
    void setPointSize(const float size);
protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;

private:
    const float DEFAULT_POINT_SIZE = 15;

    GLuint vao;
    GLuint positionBuffer;
    QOpenGLShaderProgram* shader;

    std::vector<float> positions;
    float _pointSize = DEFAULT_POINT_SIZE;
};

#endif // SCATTERPLOT_DRAWER_H
