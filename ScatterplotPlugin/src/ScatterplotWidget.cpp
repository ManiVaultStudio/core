#include "ScatterplotWidget.h"

#include <vector>

#include <QSize>
#include <QDebug>

ScatterplotWidget::ScatterplotWidget()
    :
    _densityRenderer(DensityRenderer::RenderMode::DENSITY),
    _colormapWidget(this)
{
    addSelectionListener(&_selectionRenderer);
}

void ScatterplotWidget::setRenderMode(RenderMode renderMode)
{
    _renderMode = renderMode;

    update();
}

void ScatterplotWidget::renderModePicked(const int index)
{
    switch (index)
    {
    case 0: setRenderMode(ScatterplotWidget::RenderMode::SCATTERPLOT); break;
    case 1: setRenderMode(ScatterplotWidget::RenderMode::DENSITY); break;
    case 2: setRenderMode(ScatterplotWidget::RenderMode::LANDSCAPE); break;
    }
    qDebug() << "Render Mode Picked";
}

void ScatterplotWidget::pointSizeChanged(const int size)
{
    _pointRenderer.setPointSize(size / 1000.0f);
    update();
}

void ScatterplotWidget::sigmaChanged(const int sigma)
{
    _densityRenderer.setSigma(sigma / 100.0f);
    update();
}

void ScatterplotWidget::colormapChanged(QString colormapName)
{
    //_renderer->setColormap(idx);
    _pointRenderer.setColormap(colormapName);
    update();
}

void ScatterplotWidget::colormapdiscreteChanged(bool isDiscrete)
{
    //_renderer->setColormapDiscrete(isDiscrete);

    update();
}

// Positions need to be passed as a pointer as we need to store them locally in order
// to be able to find the subset of data that's part of a selection. If passed
// by reference then we can upload the data to the GPU, but not store it in the widget.
void ScatterplotWidget::setData(const std::vector<Vector2f>* points, const QRectF bounds)
{

    float width = fabs(bounds.width());
    float height = fabs(bounds.height());
    float size = width > height ? width : height;
    Vector2f center((bounds.left() + bounds.right()) / 2, (bounds.bottom() + bounds.top()) / 2);
    QRectF uniformBounds;
    uniformBounds.setLeft(center.x - size / 2);
    uniformBounds.setRight(center.x + size / 2);
    uniformBounds.setBottom(center.y - size / 2);
    uniformBounds.setTop(center.y + size / 2);
    _dataBounds = uniformBounds;

    _pointRenderer.setBounds(_dataBounds.left(), _dataBounds.right(), _dataBounds.bottom(), _dataBounds.top());
    _pointRenderer.setData(points);
    _densityRenderer.setData(points);

    update();
}
    
void ScatterplotWidget::setHighlights(const std::vector<char>& highlights)
{
    _pointRenderer.setHighlight(highlights);

    update();
}

void ScatterplotWidget::setScalarProperty(const std::vector<float>& scalarProperty)
{
    _pointRenderer.setScalarProperty(scalarProperty);
            
    update();
}

void ScatterplotWidget::setPointSize(const float size)
{
    _pointRenderer.setPointSize(size);

    update();
}

void ScatterplotWidget::setAlpha(const float alpha)
{
    _pointRenderer.setAlpha(alpha);
}

void ScatterplotWidget::setPointScaling(hdps::gui::PointScaling scalingMode)
{
    _pointRenderer.setPointScaling(scalingMode);
}

void ScatterplotWidget::setSigma(const float sigma)
{
    _densityRenderer.setSigma(sigma);

    update();
}

void ScatterplotWidget::addSelectionListener(plugin::SelectionListener* listener)
{
    _selectionListeners.push_back(listener);
}

float lerp(float v0, float v1, float t) {
    return (1 - t) * v0 + t * v1;
}

Selection ScatterplotWidget::getSelection()
{
    Selection isotropicSelection = toIsotropicCoordinates * _selection;

    Vector2f bottomLeftData(lerp(_dataBounds.left(), _dataBounds.right(), isotropicSelection.getLeft()), lerp(_dataBounds.bottom(), _dataBounds.top(), isotropicSelection.getBottom()));
    Vector2f topRightData(lerp(_dataBounds.left(), _dataBounds.right(), isotropicSelection.getRight()), lerp(_dataBounds.bottom(), _dataBounds.top(), isotropicSelection.getTop()));
            
    return Selection(bottomLeftData, topRightData);
}

void ScatterplotWidget::initializeGL()
{
    qDebug() << "Initializing scatterplot";

    initializeOpenGLFunctions();

    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &ScatterplotWidget::cleanup);

    _colormapWidget.move(width() - 71, 10);
    _colormapWidget.show();

    QObject::connect(&_colormapWidget, &ColormapWidget::colormapSelected, this, &ScatterplotWidget::colormapChanged);
    QObject::connect(&_colormapWidget, &ColormapWidget::discreteSelected, this, &ScatterplotWidget::colormapdiscreteChanged);

    _pointRenderer.init();
    _densityRenderer.init();
    _selectionRenderer.init();

    _pointRenderer.setScalarEffect(PointEffect::Color);
    _pointRenderer.setColormap(":colormaps/Spectral.png");
}

void ScatterplotWidget::resizeGL(int w, int h)
{
    qDebug() << "Resizing scatterplot";
    _windowSize.setWidth(w);
    _windowSize.setHeight(h);

    _pointRenderer.resize(QSize(w, h));
    _densityRenderer.resize(QSize(w, h));
    _selectionRenderer.resize(QSize(w, h));

    toNormalisedCoordinates = Matrix3f(1.0f / w, 0, 0, 1.0f / h, 0, 0);

    int size = w < h ? w : h;

    float wAspect = (float)w / size;
    float hAspect = (float)h / size;
    float wDiff = ((wAspect - 1) / 2.0);
    float hDiff = ((hAspect - 1) / 2.0);

    toIsotropicCoordinates = Matrix3f(wAspect, 0, 0, hAspect, -wDiff, -hDiff);
    qDebug() << "Done resizing scatterplot";

    if (_colormapWidget._isOpen)
    {
        _colormapWidget.move(width() - (64 + 14 + 15 * 36 + 15), 10);
    }
    else {
        _colormapWidget.move(width() - 71, 10);
    }
    _colormapWidget.setColormap(0, true);
}

void ScatterplotWidget::paintGL()
{
    // Bind the framebuffer belonging to the widget
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());

    // Clear the widget to the background color
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset the blending function
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    switch (_renderMode)
    {
    case SCATTERPLOT: _pointRenderer.render(); break;
    case DENSITY:
    {
        _densityRenderer.setRenderMode(DensityRenderer::DENSITY);
        _densityRenderer.render();
        break;
    }
    case LANDSCAPE:
    {
        _densityRenderer.setRenderMode(DensityRenderer::LANDSCAPE);
        _densityRenderer.render();
        break;
    }
    }
    _selectionRenderer.render();
}

void ScatterplotWidget::mousePressEvent(QMouseEvent *event)
{
    _selecting = true;

    Vector2f point = toNormalisedCoordinates * Vector2f(event->x(), _windowSize.height() - event->y());
    _selection.setStart(point);
}

void ScatterplotWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!_selecting) return;

    Vector2f point = toNormalisedCoordinates * Vector2f(event->x(), _windowSize.height() - event->y());
    _selection.setEnd(point);

    onSelecting(_selection);

    update();
}

void ScatterplotWidget::mouseReleaseEvent(QMouseEvent *event)
{
    _selecting = false;

    Vector2f point = toNormalisedCoordinates * Vector2f(event->x(), _windowSize.height() - event->y());
    _selection.setEnd(point);

    onSelection(_selection);

    update();
}

void ScatterplotWidget::onSelecting(Selection selection)
{
    _selection.set(selection.getStart(), selection.getEnd());

    for (plugin::SelectionListener* listener : _selectionListeners)
        listener->onSelecting(selection);

    update();
}

void ScatterplotWidget::onSelection(Selection selection)
{
    _selection.set(selection.getStart(), selection.getEnd());

    for (plugin::SelectionListener* listener : _selectionListeners)
        listener->onSelection(_selection);

    update();
}

void ScatterplotWidget::cleanup()
{
    qDebug() << "Deleting scatterplot widget, performing clean up...";
    makeCurrent();

    _pointRenderer.destroy();
    _densityRenderer.destroy();
    _selectionRenderer.destroy();
}
