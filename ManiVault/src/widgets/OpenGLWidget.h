#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QDropEvent>
#include <QMoveEvent>

namespace mv
{
namespace gui
{

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    OpenGLWidget();
    ~OpenGLWidget();

protected:
    void initializeGL()         Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    void paintGL()              Q_DECL_OVERRIDE;
    void cleanup();

protected:
    virtual void onWidgetInitialized()          = 0;
    virtual void onWidgetResized(int w, int h)  = 0;
    virtual void onWidgetRendered()             = 0;
    virtual void onWidgetCleanup()              = 0;

private:
    void showEvent(QShowEvent* showEvent) Q_DECL_OVERRIDE;

private slots:
    void updatePixelRatio();

private:
    float   _pixelRatio;        /** Pixel ratio of the screen the widget is displayed on */
};

} // namespace gui
} // namespace mv
