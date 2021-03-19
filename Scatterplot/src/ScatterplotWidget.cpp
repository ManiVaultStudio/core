#include "ScatterplotWidget.h"
#include "PixelSelectionTool.h"
#include "Application.h"

#include "util/Math.h"

#include <vector>

#include <QSize>
#include <QDebug>
#include <math.h>

using namespace hdps::util;

namespace
{
    Bounds getDataBounds(const std::vector<Vector2f>& points)
    {
        Bounds bounds = Bounds::Max;

        for (const Vector2f& point : points)
        {
            bounds.setLeft(std::min(point.x, bounds.getLeft()));
            bounds.setRight(std::max(point.x, bounds.getRight()));
            bounds.setBottom(std::min(point.y, bounds.getBottom()));
            bounds.setTop(std::max(point.y, bounds.getTop()));
        }

        return bounds;
    }
}

ScatterplotWidget::ScatterplotWidget(PixelSelectionTool& pixelSelectionTool) :
    _densityRenderer(DensityRenderer::RenderMode::DENSITY),
    _colormapWidget(this),
    _pointRenderer(),
    _pixelSelectionToolRenderer(pixelSelectionTool),
    _pixelSelectionTool(pixelSelectionTool)
{
    setContextMenuPolicy(Qt::CustomContextMenu);

    this->installEventFilter(&_pixelSelectionTool);

    QObject::connect(&_pixelSelectionTool, &PixelSelectionTool::shapeChanged, [this]() {
        if (!isInitialized() || _renderMode != SCATTERPLOT)
            return;

        _pixelSelectionToolRenderer.update();
        update();
    });

    _pointRenderer.setPointScaling(Absolute);
}

bool ScatterplotWidget::isInitialized()
{
    return _isInitialized;
}

ScatterplotWidget::RenderMode ScatterplotWidget::getRenderMode() const
{
    return _renderMode;
}

void ScatterplotWidget::setRenderMode(const RenderMode& renderMode)
{
    if (renderMode == _renderMode)
        return;

    _renderMode = renderMode;

    emit renderModeChanged(_renderMode);

    switch (_renderMode)
    {
        case ScatterplotWidget::SCATTERPLOT:
            break;
        
        case ScatterplotWidget::DENSITY:
            computeDensity();
            break;

        case ScatterplotWidget::LANDSCAPE:
            computeDensity();
            break;

        default:
            break;
    }

    update();
}

ScatterplotWidget::ColoringMode ScatterplotWidget::getColoringMode() const
{
    return _coloringMode;
}

void ScatterplotWidget::setColoringMode(const ColoringMode& coloringMode)
{
    if (coloringMode == _coloringMode)
        return;

    _coloringMode = coloringMode;

    emit coloringModeChanged(_coloringMode);
}

void ScatterplotWidget::computeDensity()
{
    emit densityComputationStarted();

    _densityRenderer.computeDensity();

    emit densityComputationEnded();

    update();
}

void ScatterplotWidget::colormapChanged(QString colormapName)
{
    _pointRenderer.setColormap(colormapName);
    _densityRenderer.setColormap(colormapName);
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
void ScatterplotWidget::setData(const std::vector<Vector2f>* points)
{
    auto dataBounds = getDataBounds(*points);

    dataBounds.ensureMinimumSize(1e-07f, 1e-07f);
    dataBounds.makeSquare();
    dataBounds.expand(0.1f);

    _dataBounds = dataBounds;

    // Pass bounds and data to renderers
    _pointRenderer.setBounds(_dataBounds);
    _densityRenderer.setBounds(_dataBounds);

    _pointRenderer.setData(*points);
    _densityRenderer.setData(points);

    switch (_renderMode)
    {
        case ScatterplotWidget::SCATTERPLOT:
            break;
        
        case ScatterplotWidget::DENSITY:
        case ScatterplotWidget::LANDSCAPE:
        {
            _densityRenderer.computeDensity();
            break;
        }

        default:
            break;
    }

    _pointRenderer.setOutlineColor(Vector3f(1, 0, 0));

    update();
}

void ScatterplotWidget::setHighlights(const std::vector<char>& highlights)
{
    _pointRenderer.setHighlights(highlights);

    update();
}

void ScatterplotWidget::setScalars(const std::vector<float>& scalars)
{
    _pointRenderer.setScalars(scalars);
    
    update();
}

void ScatterplotWidget::setColors(const std::vector<Vector3f>& colors)
{
    _pointRenderer.setColors(colors);
    _pointRenderer.setScalarEffect(None);

    update();
}

void ScatterplotWidget::setPointSize(const float& pointSize)
{
    _pointRenderer.setPointSize(pointSize);

    update();
}

void ScatterplotWidget::setAlpha(const float alpha)
{
    _pointRenderer.setAlpha(alpha);

    update();
}

void ScatterplotWidget::setPointScaling(hdps::gui::PointScaling scalingMode)
{
    _pointRenderer.setPointScaling(scalingMode);
}

void ScatterplotWidget::setScalarEffect(PointEffect effect)
{
    _pointRenderer.setScalarEffect(effect);
}

void ScatterplotWidget::setSigma(const float sigma)
{
    _densityRenderer.setSigma(sigma);

    update();
}

void ScatterplotWidget::initializeGL()
{
    initializeOpenGLFunctions();
    qDebug() << "Initializing scatterplot widget with context: " << context();
    std::string versionString = std::string((const char*) glGetString(GL_VERSION));
    qDebug() << versionString.c_str();

    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &ScatterplotWidget::cleanup);

    _colormapWidget.move(width() - 71, 10);
    _colormapWidget.show();

    QObject::connect(&_colormapWidget, &ColormapWidget::colormapSelected, this, &ScatterplotWidget::colormapChanged);
    QObject::connect(&_colormapWidget, &ColormapWidget::discreteSelected, this, &ScatterplotWidget::colormapdiscreteChanged);

    _pointRenderer.init();
    _densityRenderer.init();
    _pixelSelectionToolRenderer.init();

    // Set a default color map for both renderers
    _pointRenderer.setScalarEffect(PointEffect::Color);
    _pointRenderer.setColormap(_colormapWidget.getActiveColormap());
    _densityRenderer.setColormap(_colormapWidget.getActiveColormap());

    _isInitialized = true;
    emit initialized();

}

void ScatterplotWidget::resizeGL(int w, int h)
{
    _windowSize.setWidth(w);
    _windowSize.setHeight(h);

    _pointRenderer.resize(QSize(w, h));
    _densityRenderer.resize(QSize(w, h));
    _pixelSelectionToolRenderer.resize(QSize(w, h));

    // Set matrix for normalizing from pixel coordinates to [0, 1]
    toNormalisedCoordinates = Matrix3f(1.0f / w, 0, 0, 1.0f / h, 0, 0);

    // Take the smallest dimensions in order to calculate the aspect ratio
    int size = w < h ? w : h;

    float wAspect = (float)w / size;
    float hAspect = (float)h / size;
    float wDiff = ((wAspect - 1) / 2.0);
    float hDiff = ((hAspect - 1) / 2.0);

    
    toIsotropicCoordinates = Matrix3f(wAspect, 0, 0, hAspect, -wDiff, -hDiff);

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
        case SCATTERPLOT:
        {
            _pointRenderer.render();
            _pixelSelectionToolRenderer.render();
            break;
        }

        case DENSITY:
        case LANDSCAPE:
        {
            _densityRenderer.setRenderMode(_renderMode == DENSITY ? DensityRenderer::DENSITY : DensityRenderer::LANDSCAPE);
            _densityRenderer.render();
            break;
        }
    }
}

void ScatterplotWidget::cleanup()
{
    qDebug() << "Deleting scatterplot widget, performing clean up...";
    _isInitialized = false;

    makeCurrent();
    _pointRenderer.destroy();
    _densityRenderer.destroy();
    _pixelSelectionToolRenderer.destroy();
}

ScatterplotWidget::~ScatterplotWidget()
{
    disconnect(QOpenGLWidget::context(), &QOpenGLContext::aboutToBeDestroyed, this, &ScatterplotWidget::cleanup);
    cleanup();
}