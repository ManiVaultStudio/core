#pragma once

#include "renderers/PointRenderer.h"
#include "renderers/DensityRenderer.h"
#include "renderers/SelectionRenderer.h"

#include "SelectionListener.h"

#include "graphics/Vector2f.h"
#include "graphics/Vector3f.h"
#include "graphics/Matrix3f.h"
#include "graphics/Bounds.h"
#include "graphics/Selection.h"

#include "widgets/ColormapWidget.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>

#include <QMouseEvent>

using namespace hdps;
using namespace hdps::gui;

class ScatterplotWidget : public QOpenGLWidget, QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    enum RenderMode {
        SCATTERPLOT, DENSITY, LANDSCAPE
    };

    ScatterplotWidget();

    ~ScatterplotWidget();

    /** Returns true when the widget was initialized and is ready to be used. */
    bool isInitialized();

    /**
     * Change the rendering mode displayed in the widget.
     * The options are defined by ScatterplotWidget::RenderMode.
     */
    void setRenderMode(RenderMode renderMode);

    /**
     * Feed 2-dimensional data to the scatterplot.
     */
    void setData(const std::vector<Vector2f>* data);
    void setHighlights(const std::vector<char>& highlights);
    void setScalars(const std::vector<float>& scalars);
    void setColors(const std::vector<Vector3f>& colors);

    void setPointSize(const float size);
    void setScalarEffect(PointEffect effect);
    void setAlpha(const float alpha);
    void setPointScaling(hdps::gui::PointScaling scalingMode);
    void setSigma(const float sigma);
    void addSelectionListener(plugin::SelectionListener* listener);

    Selection getSelection();

    /**
     * Event filter
     *@param target Target object
     *@param event Event that occurred
     */
    bool eventFilter(QObject* target, QEvent* event) override;

protected:
    void initializeGL()         Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    void paintGL()              Q_DECL_OVERRIDE;

    void mousePressEvent(QMouseEvent *event)   Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event)    Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

    void onSelecting(Selection selection);
    void onSelection(Selection selection);
    void cleanup();

signals:
    void initialized();

public slots:
    void renderModePicked(const int index);
    void pointSizeChanged(const int size);
    void pointOpacityChanged(const int opacity);
    void sigmaChanged(const int sigma);
    void computeDensity();

    void colormapChanged(QString colormapName);
    void colormapdiscreteChanged(bool isDiscrete);

private:
    const Matrix3f toClipCoordinates = Matrix3f(2, 0, 0, 2, -1, -1);
    Matrix3f toNormalisedCoordinates;
    Matrix3f toIsotropicCoordinates;

    bool _isInitialized = false;

    RenderMode _renderMode = SCATTERPLOT;

    /* Renderers */
    PointRenderer _pointRenderer;
    DensityRenderer _densityRenderer;
    SelectionRenderer _selectionRenderer;


    /* Auxiliary widgets */
    ColormapWidget _colormapWidget;

    /* Selection */
    bool _selecting = false;
    Selection _selection;
    std::vector<plugin::SelectionListener*> _selectionListeners;

    /* Size of the scatterplot widget */
    QSize _windowSize;

    Bounds _dataBounds;
};
