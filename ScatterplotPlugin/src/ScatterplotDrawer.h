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

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;

private:
    GLuint vao;
    QOpenGLShaderProgram shader;
};

#endif // SCATTERPLOT_DRAWER_H
