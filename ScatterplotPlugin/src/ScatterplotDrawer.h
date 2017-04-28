#ifndef SCATTERPLOT_DRAWER_H
#define SCATTERPLOT_DRAWER_H

#include <QWidget>
#include <QOpenGLWidget>

class ScatterplotDrawer : public QOpenGLWidget
{
public:
    ScatterplotDrawer(QWidget* parent = 0);

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
};

#endif // SCATTERPLOT_DRAWER_H
