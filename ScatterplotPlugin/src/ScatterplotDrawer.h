#ifndef SCATTERPLOT_DRAWER_H
#define SCATTERPLOT_DRAWER_H

#include <QObject>
#include <QWidget>
#include <QOpenGLWidget>

class ScatterplotDrawer : public QOpenGLWidget
{
    Q_OBJECT
public:

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
};

#endif // SCATTERPLOT_DRAWER_H
