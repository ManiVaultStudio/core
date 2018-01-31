#include "DensityPlotWidget.h"

#include <vector>
#include <algorithm>
#include <QDebug>

#define GLSL(version, shader)  "#version " #version "\n" #shader

namespace hdps
{
namespace gui
{

// Positions need to be passed as a pointer as we need to store them locally in order
// to be able to find the subset of data that's part of a selection. If passed
// by reference then we can upload the data to the GPU, but not store it in the widget.
void DensityPlotWidget::setData(const std::vector<Vector2f>* positions)
{
    _numPoints = (unsigned int)positions->size();
    _positions = positions;

    qDebug() << "Setting position data";
    meanShift.setData(_positions);

    _needsDensityMapUpdate = true;
    update();
}

void DensityPlotWidget::addSelectionListener(const plugin::SelectionListener* listener)
{
    _selectionListeners.push_back(listener);
}

void DensityPlotWidget::initializeGL()
{
    initializeOpenGLFunctions();

    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &DensityPlotWidget::cleanup);

    glClearColor(1, 1, 1, 1);
    qDebug() << "Initializing density plot";

    meanShift.init();

    if (_numPoints > 0)
    {
        meanShift.setData(_positions);
    }

    bool loaded = _shaderDensityDraw.loadShaderFromFile(":shaders/Quad.vert", ":shaders/DensityDraw.frag");
    if (!loaded) {
        qDebug() << "Failed to load DensityDraw shader";
    }

    loaded = _shaderGradientDraw.loadShaderFromFile(":shaders/Quad.vert", ":shaders/GradientDraw.frag");
    if (!loaded) {
        qDebug() << "Failed to load GradientDraw shader";
    }

    loaded = _shaderMeanShiftDraw.loadShaderFromFile(":shaders/Quad.vert", ":shaders/Texture.frag");
    if (!loaded) {
        qDebug() << "Failed to load MeanShiftDraw shader";
    }
}

void DensityPlotWidget::resizeGL(int w, int h)
{
    _windowSize.setWidth(w);
    _windowSize.setHeight(h);

    int size = w < h ? w : h;

    float wAspect = (float)w / size;
    float hAspect = (float)h / size;
    float wDiff = ((wAspect - 1) / 2.0);
    float hDiff = ((hAspect - 1) / 2.0);

    toNormalisedCoordinates = Matrix3f(1.0f / w, 0, 0, 1.0f / h, 0, 0);
    toIsotropicCoordinates = Matrix3f(wAspect, 0, 0, hAspect, -wDiff, -hDiff);
}

void DensityPlotWidget::paintGL()
{
    qDebug() << "Drawing density plot";

    if (_needsDensityMapUpdate)
    {
        meanShift.computeDensity();
        _needsDensityMapUpdate = false;
    }
    
    meanShift.computeGradient();
    meanShift.computeMeanShift();

    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
    glViewport(0, 0, _windowSize.width(), _windowSize.height());

    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //drawDensity();
    //drawGradient();
    drawMeanShift();
}

void DensityPlotWidget::drawGradient()
{
    glViewport(0, 0, _windowSize.width(), _windowSize.height());

    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (_numPoints > 0) {
        _shaderGradientDraw.bind();

        _gradientTexture.bind(0);
        _shaderGradientDraw.uniform1i("tex", 0);

        drawFullscreenQuad();

        _shaderGradientDraw.release();
    }
}

void DensityPlotWidget::cluster()
{
    if (_vtxIdxs.size() <= 0) return;

    _clusterIds.resize(_msTexSize * _msTexSize);
    _clusterIdsOriginal.resize(_msTexSize * _msTexSize);
    std::vector< std::vector< float > > clusterCenters;

    float epsilon = (float)_msTexSize / (128 * 256);
    for (int i = 0; i < _meanShiftMapCPU.size(); i += 3) {

        std::vector< float > center = { _meanShiftMapCPU[i], _meanShiftMapCPU[i + 1] };

        if (center[0] < 0.00001 && center[1] < 0.00001)
        {
            _clusterIdsOriginal[i / 3] = -1;
            continue;
        }

        int clusterId = -1;
        for (int c = 0; c < clusterCenters.size(); c++)
        {
            if (equal(center, clusterCenters[c], epsilon))
            {
                clusterId = c;
                break;
            }
        }

        if (clusterId < 0)
        {
            clusterCenters.push_back(center);
            _clusterIdsOriginal[i / 3] = static_cast<int>(clusterCenters.size() - 1);
        }
        else
        {
            _clusterIdsOriginal[i / 3] = clusterId;
        }

        //std::cout << center[0] << ", " << center[1];
    }

    // DEBUG ======================================================================
    QImage pluto(_msTexSize, _msTexSize, QImage::Format::Format_ARGB32);
    float scale = 255.0 / clusterCenters.size();
    for (int j = 0; j < _msTexSize; ++j)
    {
    	for (int i = 0; i < _msTexSize; ++i)
    	{
    		int idx = j * _msTexSize + i;
    		pluto.setPixel(i, j, qRgb(_clusterIds[idx] * scale, _clusterIds[idx] * scale, _clusterIds[idx] * scale));
    	}
    }
    pluto.save("meanshift_clusters.png");
    qDebug() << "Saved image of clusters";
    // DEBUG ======================================================================

    //std::vector< std::vector< std::pair <int, int> > > mcvClusters(clusterCenters.size());
    std::vector<int> activeIds(clusterCenters.size(), -1);
    int runningIdx = 0;
    _clusterPositions.clear();
    for (int i = 0; i < _vtxIdxs.size() / 2; i++) {

        int x = (int)(_vtxIdxs[2 * i] * (_msTexSize - 1) + 0.5);
        int y = (int)(_vtxIdxs[2 * i + 1] * (_msTexSize - 1) + 0.5);

        int idx = (x + y * _msTexSize);

        int cId = _clusterIdsOriginal[idx];
        if (cId >= 0 && activeIds[cId] < 0) {

            std::vector< float > center = { clusterCenters[cId] };
            _clusterPositions.push_back(center[0]);
            _clusterPositions.push_back(center[1]);

            activeIds[cId] = runningIdx++;
        }
    }

#pragma omp parallel for
    for (int i = 0; i < _clusterIdsOriginal.size(); i++) {
        if (_clusterIdsOriginal[i] >= 0){ _clusterIdsOriginal[i] = activeIds[_clusterIdsOriginal[i]]; }
        _clusterIds[i] = _clusterIdsOriginal[i];
    }

    std::vector< std::vector<unsigned int> > clusterIdxs(runningIdx);
    for (int i = 0; i < _vtxIdxs.size() / 2; i++) {

        int x = (int)(_vtxIdxs[2 * i] * (_msTexSize - 1) + 0.5);
        int y = (int)(_vtxIdxs[2 * i + 1] * (_msTexSize - 1) + 0.5);

        int idx = (x + y * _msTexSize);

        int cId = _clusterIdsOriginal[idx];
        if (cId >= 0) { clusterIdxs[cId].push_back(i); }
        ////mcvClusters[cId].push_back(_localPointReferences[i]);
    }

    ////_meanShiftClusters = MCV_CytometryData::Instance()->derivedDataClusters("Density Clusters - " + _activeDataName);
    ////_meanShiftClusters->setClusters(mcvClusters);

    //>>>_meanShiftClusters = _analysis->updateClusters(&clusterIdxs, "Density Clusters");

    //>>>refreshClusterBuffers();

    //>>>_clustersNeedRefresh = false;
}

void DensityPlotWidget::drawMeanShift()
void DensityPlotWidget::createSampleSelectionTextureBuffer()
{
    if (_numPoints == 0) return;

    _shaderMeanShiftDraw.bind();

    meanShift.getMeanShiftTexture().bind(0);
    _shaderMeanShiftDraw.uniform1i("tex", 0);

    meanShift.drawFullscreenQuad();
}

bool DensityPlotWidget::equal(const std::vector<float> &p1, const std::vector<float> &p2, float epsilon)
{
    if (p1.size() != p2.size()) return false;

    bool equal = true;

    //std::cout << "\nComparing Points: (" << p1[0] << ", " << p1[1] << ") and (" << p2[0] << ", " << p2[1] << ") with epsilon " << epsilon << ".\n";

    for (int i = 0; i < p1.size(); i++)
    {
        equal = fabs(p1[i] - p2[i]) < epsilon;
        //std::cout << "	" << i << ": abs(" << p1[i] << " - " << p2[i] << ") = " << std::abs(p1[i] - p2[i]) << " < " << epsilon << "? " << (equal ? "true" : "false\n");

        if (!equal) break;
    }

    return equal;
}


void DensityPlotWidget::mousePressEvent(QMouseEvent *event)
{
    _selecting = true;

    Vector2f point = toNormalisedCoordinates * Vector2f(event->x(), _windowSize.height() - event->y());
    _selection.setStart(point);
}

void DensityPlotWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!_selecting) return;

    Vector2f point = toNormalisedCoordinates * Vector2f(event->x(), _windowSize.height() - event->y());
    _selection.setEnd(point);

    update();
}

void DensityPlotWidget::mouseReleaseEvent(QMouseEvent *event)
{
    _selecting = false;

    Vector2f point = toNormalisedCoordinates * Vector2f(event->x(), _windowSize.height() - event->y());
    _selection.setEnd(point);

    onSelection(_selection);
}

void DensityPlotWidget::onSelection(Selection selection)
{
    update();

    std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < _numPoints; i++)
    {
        Vector2f point = (*_positions)[i];
        point.x *= _windowSize.width() / _windowSize.height();

        if (selection.contains(point))
            indices.push_back(i);
    }

    for (const plugin::SelectionListener* listener : _selectionListeners)
        listener->onSelection(indices);
}

void DensityPlotWidget::cleanup()
{
    qDebug() << "Deleting density plot widget, performing clean up...";
    makeCurrent();

    // Delete objects
    meanShift.cleanup();
}

} // namespace gui

} // namespace hdps
