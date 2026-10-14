#pragma once

#include "ManiVaultGlobals.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QDropEvent>
#include <QMoveEvent>

namespace mv
{
namespace gui
{

class CORE_EXPORT OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    OpenGLWidget();
    OpenGLWidget(int glMajorVersion, int glMinorVersion);
    ~OpenGLWidget();

    bool isWidgetInitialized() { return _isInitialized; }

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

signals:
    void widgetInitialized();

private slots:
    void updatePixelRatio();

private:
    float   _pixelRatio;        /** Pixel ratio of the screen the widget is displayed on */
    bool    _isInitialized;     /** Whether the widget is ready for OpenGL operations */
};

} // namespace gui
} // namespace mv
