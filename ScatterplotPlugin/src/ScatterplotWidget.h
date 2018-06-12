#pragma once

#include "widgets/PointRenderer.h"
#include "widgets/DensityRenderer.h"
#include "widgets/SelectionRenderer.h"

#include "SelectionListener.h"

#include "graphics/BufferObject.h"
#include "graphics/Vector2f.h"
#include "graphics/Vector3f.h"
#include "graphics/Matrix3f.h"
#include "graphics/Selection.h"
#include "graphics/Shader.h"

#include "widgets/ColorMapWidget.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>

#include <QMouseEvent>
#include <memory>

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
    void setRenderMode(RenderMode renderMode);
    void setData(const std::vector<Vector2f>* data, const QRectF bounds);
    void setColors(const std::vector<Vector3f>& data);
    void setHighlights(const std::vector<char>& highlights);
    void setScalarProperty(const std::vector<float>& scalarProperty);
    void setPointSize(const float size);
    void setAlpha(const float alpha);
    void setPointScaling(hdps::gui::PointScaling scalingMode);
    void setSigma(const float sigma);
    void addSelectionListener(plugin::SelectionListener* listener);

    Selection getSelection();
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

protected slots:
    void renderModePicked(const int index);
    void pointSizeChanged(const int size);
    void sigmaChanged(const int sigma);

    void colormapChanged(int idx);
    void colormapdiscreteChanged(bool isDiscrete);

private:
    const Matrix3f toClipCoordinates = Matrix3f(2, 0, 0, 2, -1, -1);
    Matrix3f toNormalisedCoordinates;
    Matrix3f toIsotropicCoordinates;

    RenderMode _renderMode = SCATTERPLOT;

    PointRenderer _pointRenderer;
    DensityRenderer _densityRenderer;
    SelectionRenderer _selectionRenderer;

    QSize _windowSize;

    ColorMapWidget _colorMapWidget;

    unsigned int _numPoints = 0;
    QRectF _dataBounds;

    bool _selecting = false;
    Selection _selection;
    std::vector<plugin::SelectionListener*> _selectionListeners;
};
