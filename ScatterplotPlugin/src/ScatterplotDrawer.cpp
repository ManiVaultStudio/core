#include "ScatterplotDrawer.h"

#include <QOpenGLFunctions>
#include <QDebug>

void ScatterplotDrawer::initializeGL()
{
    QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();

    f->glClearColor(1.0, 0.0, 0.0, 1.0);
    qDebug() << "Initializing scatterplot";
}

void ScatterplotDrawer::resizeGL(int w, int h)
{

}

void ScatterplotDrawer::paintGL()
{
    QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();

    f->glClear(GL_COLOR_BUFFER_BIT);
    qDebug() << "Rendering scatterplot";
}
