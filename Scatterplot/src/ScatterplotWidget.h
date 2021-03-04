#pragma once

#include "renderers/PointRenderer.h"
#include "renderers/DensityRenderer.h"

#include "graphics/Vector2f.h"
#include "graphics/Vector3f.h"
#include "graphics/Matrix3f.h"
#include "graphics/Bounds.h"
#include "graphics/Selection.h"

#include "widgets/ColormapWidget.h"

#include "PixelSelectionToolRenderer.h"
#include "DropDataTypesWidget.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>

#include <QMouseEvent>
#include <QMenu>

class PixelSelectionTool;

using namespace hdps;
using namespace hdps::gui;

class ScatterplotWidget : public QOpenGLWidget, QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    enum RenderMode {
        SCATTERPLOT,
        DENSITY,
        LANDSCAPE
    };

    /** The way that point colors are determined */
    enum class ColoringMode {
        ConstantColor,      /** Point color is a constant color */
        ColorDimension,          /** Data dimension drives the color */
        ColorData           /** Determined by external color dataset */
    };

public:
    ScatterplotWidget(PixelSelectionTool& pixelSelectionTool);
    ~ScatterplotWidget();

    /** Returns true when the widget was initialized and is ready to be used. */
    bool isInitialized();

    /** Get/set render mode */
    RenderMode getRenderMode() const;
    void setRenderMode(const RenderMode& renderMode);

    /** Get/set coloring mode */
    ColoringMode getColoringMode() const;
    void setColoringMode(const ColoringMode& coloringMode);

    /**
     * Feed 2-dimensional data to the scatterplot.
     */
    void setData(const std::vector<Vector2f>* data);
    void setHighlights(const std::vector<char>& highlights);
    void setScalars(const std::vector<float>& scalars);

    /**
     * Set colors for each individual data point
     * @param colors Vector of colors (size must match that of the loaded points dataset)
     */
    void setColors(const std::vector<Vector3f>& colors);

    /**
     * Set point size in pixels
     * @param pointSize Point size in pixels
     */
    void setPointSize(const float& pointSize);
    void setScalarEffect(PointEffect effect);
    void setAlpha(const float alpha);
    void setPointScaling(hdps::gui::PointScaling scalingMode);
    void setSigma(const float sigma);

    Bounds getBounds() const {
        return _dataBounds;
    }

protected:
    void initializeGL()         Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    void paintGL()              Q_DECL_OVERRIDE;

    void cleanup();

signals:
    void initialized();

    /**
     * Signals that the render mode changed
     * @param renderMode Signals that the render mode has changed
     */
    void renderModeChanged(const RenderMode& renderMode);

    /**
     * Signals that the coloring mode changed
     * @param coloringMode Signals that the coloring mode has changed
     */
    void coloringModeChanged(const ColoringMode& coloringMode);

    /** Signals that the density computation has started */
    void densityComputationStarted();

    /** Signals that the density computation has ended */
    void densityComputationEnded();

public slots:
    void computeDensity();

    void colormapChanged(QString colormapName);
    void colormapdiscreteChanged(bool isDiscrete);

private:
    const Matrix3f              toClipCoordinates = Matrix3f(2, 0, 0, 2, -1, -1);
    Matrix3f                    toNormalisedCoordinates;
    Matrix3f                    toIsotropicCoordinates;
    bool                        _isInitialized = false;
    RenderMode                  _renderMode = SCATTERPLOT;
    ColoringMode                _coloringMode = ColoringMode::ConstantColor;
    PointRenderer               _pointRenderer;                     
    DensityRenderer             _densityRenderer;                   
    PixelSelectionToolRenderer  _pixelSelectionToolRenderer;        
    ColormapWidget              _colormapWidget;                    
    DropDataTypesWidget              _dropDataWidget;                    
    QSize                       _windowSize;                        /** Size of the scatterplot widget */
    Bounds                      _dataBounds;                        /** Bounds of the loaded data */
    PixelSelectionTool&         _pixelSelectionTool;                
};