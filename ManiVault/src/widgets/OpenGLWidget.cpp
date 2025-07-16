#include "OpenGLWidget.h"

#include <QSurfaceFormat>
#include <QWindow>
#include <QScreen>

namespace
{
    /** Return the format necessary for rendering with PointRenderer class */
    QSurfaceFormat getNecessarySurfaceFormat(int glMajorVersion, int glMinorVersion)
    {
        QSurfaceFormat surfaceFormat;
        surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);

#ifdef __APPLE__
        // Ask for an OpenGL 3.3 Core Context as the default
        surfaceFormat.setVersion(3, 3);
        surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
        surfaceFormat.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
#else
        // Ask for an OpenGL Core Context as the default
        surfaceFormat.setVersion(glMajorVersion, glMinorVersion);
        surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
        surfaceFormat.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
#endif

        // Apply some anti-aliasing
        surfaceFormat.setSamples(16);

        return surfaceFormat;
    }
}

namespace mv
{
namespace gui
{

OpenGLWidget::OpenGLWidget() :
    OpenGLWidget(3, 3)
{}

OpenGLWidget::OpenGLWidget(int glMajorVersion, int glMinorVersion) :
    _pixelRatio(1.0f),
    _isInitialized(false)
{
    // Set the QOpenGLWidget surface format, such that we can render on it
    setFormat(getNecessarySurfaceFormat(glMajorVersion, glMinorVersion));
}

OpenGLWidget::~OpenGLWidget()
{
    // If this widget is destroyed before the OpenGL context is, then disconnect signals from the context
    disconnect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &OpenGLWidget::cleanup);

    cleanup();
}

/** Function called by QOpenGLWidget when the OpenGL context has fully initialized. */
void OpenGLWidget::initializeGL()
{
    // Initiate the OpenGL functions object, so we can use direct OpenGL commands
    initializeOpenGLFunctions();

    // If the OpenGL context associated with this widget is about to be destroyed, clean up any resources first
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &OpenGLWidget::cleanup);

    onWidgetInitialized();

    _isInitialized = true;

    emit widgetInitialized();
}

/** Function called by QOpenGLWidget when the widget has been resized. */
void OpenGLWidget::resizeGL(int w, int h)
{
    // We need this here as we do not have the screen yet to get the actual devicePixelRatio when the view is created
    _pixelRatio = devicePixelRatio();

    // Update the width and height of the widget scaled by the pixel ratio.
    // Pixel ratio might change on MacOS high dpi screens or through windows display scaling
    w *= _pixelRatio;
    h *= _pixelRatio;

    onWidgetResized(w, h);
}

/** Function called by QOpenGLWidget when the widget was instructed to repaint. */
void OpenGLWidget::paintGL()
{
    onWidgetRendered();
}

/** Called by this class to clean up any resources derived classes may be using before the OpenGL context is destroyed. */
void OpenGLWidget::cleanup()
{
    // As this function may be called when the OpenGL context associated
    // with this widget may not be active, we need to make it current first.
    makeCurrent();

    onWidgetCleanup();
}

void OpenGLWidget::showEvent(QShowEvent* showEvent)
{
    // Connect screenChanged to updating the pixel ratio
    // This is necessary in case the window is moved between high and low dpi screens
    // e.g., from a laptop display to a projector
    const QWidget* nativeParent = nativeParentWidget();
    if (nativeParent && nativeParentWidget()->windowHandle())
        connect(nativeParentWidget()->windowHandle(), &QWindow::screenChanged, this, &OpenGLWidget::updatePixelRatio);
    else
        qCritical() << "Failed to connect updatePixelRatio() in OpenGLWidget";
}

/** Checks if the pixel ratio of the screen the widget is displayed on changed and calls resizeGL() if so. */
void OpenGLWidget::updatePixelRatio()
{
    float pixelRatio = devicePixelRatio();

    // Only update if the ratio actually changed
    if (_pixelRatio != pixelRatio)
    {
        _pixelRatio = pixelRatio;
        resizeGL(width(), height());
        update();
    }
}

} // namespace gui
} // namespace mv
