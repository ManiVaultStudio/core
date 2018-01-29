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
    _positionBuffer.bind();
    _positionBuffer.setData(*positions);

    _needsDensityMapUpdate = true;
    update();
}

void GaussianTexture::generate()
{
    int w = 32;
    int h = 32;

    float* data = new float[w * h];

    double sigma = w / 6.0;
    double i_c = w / 2.0;
    double j_c = h / 2.0;

    for (int j = 0; j < h; j++){
        for (int i = 0; i < w; i++){

            const double sqrt_dist = (i - i_c)*(i - i_c) + (j - j_c)*(j - j_c);
            const double val = exp(sqrt_dist / (-2.0 * sigma * sigma)) / (2.0 * 3.1415926535 * sigma * sigma);

            data[j*w + i] = val*1000.0;// texture_multiplier;
        }
    }

    glBindTexture(GL_TEXTURE_2D, _handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, w, h, 0, GL_RED, GL_FLOAT, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    delete[] data;
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

    _gaussTexture.create();
    _gaussTexture.generate();

    glGenVertexArrays(1, &_quad);

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    std::vector<float> quad(
    {
        -1, -1, 0, 0,
        1, -1, 1, 0,
        -1, 1, 0, 1,
        -1, 1, 0, 1,
        1, -1, 1, 0,
        1, 1, 1, 1
    }
    );

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    BufferObject quadBuffer;
    quadBuffer.create();
    quadBuffer.bind();
    quadBuffer.setData(quad);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));
    glEnableVertexAttribArray(1);

    _positionBuffer.create();
    _positionBuffer.bind();
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribDivisor(2, 1);
    glEnableVertexAttribArray(2);

    if (_numPoints > 0)
    {
        _positionBuffer.bind();
        _positionBuffer.setData(*_positions);
    }

    bool loaded = _shaderDensityCompute.loadShaderFromFile(":shaders/DensityCompute.vert", ":shaders/DensityCompute.frag");
    if (!loaded) {
        qDebug() << "Failed to load DensityCompute shader";
    }

    loaded = _shaderDensityDraw.loadShaderFromFile(":shaders/Quad.vert", ":shaders/DensityDraw.frag");
    if (!loaded) {
        qDebug() << "Failed to load DensityDraw shader";
    }

    loaded = _shaderGradientCompute.loadShaderFromFile(":shaders/Quad.vert", ":shaders/GradientCompute.frag");
    if (!loaded) {
        qDebug() << "Failed to load GradientCompute shader";
    }

    loaded = _shaderGradientDraw.loadShaderFromFile(":shaders/Quad.vert", ":shaders/GradientDraw.frag");
    if (!loaded) {
        qDebug() << "Failed to load GradientDraw shader";
    }

    loaded = _shaderMeanshiftCompute.loadShaderFromFile(":shaders/Quad.vert", ":shaders/MeanshiftCompute.frag");
    if (!loaded) {
        qDebug() << "Failed to load MeanshiftCompute shader";
    }

    _meanshiftFramebuffer.create();
    _meanshiftFramebuffer.bind();

    _densityTexture.create();
    _densityTexture.bind();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, _msTexSize, _msTexSize, 0, GL_RGB, GL_FLOAT, NULL);
    
    _meanshiftFramebuffer.addColorTexture(0, &_densityTexture);
    _meanshiftFramebuffer.validate();


    _gradientTexture.create();
    _gradientTexture.bind();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, _msTexSize, _msTexSize, 0, GL_RGB, GL_FLOAT, NULL);

    _meanshiftFramebuffer.addColorTexture(1, &_gradientTexture);
    _meanshiftFramebuffer.validate();

    _meanshiftTexture.create();
    _meanshiftTexture.bind();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, _msTexSize, _msTexSize, 0, GL_RGB, GL_FLOAT, NULL);

    _meanshiftFramebuffer.addColorTexture(2, &_meanshiftTexture);
    _meanshiftFramebuffer.validate();
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
        drawDensityOffscreen();

    drawGradientOffscreen();
    drawMeanshiftOffscreen();

    //drawDensity();
    drawGradient();
}

void DensityPlotWidget::drawFullscreenQuad()
{
    glBindVertexArray(_quad);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

void DensityPlotWidget::drawDensityOffscreen()
{
    glViewport(0, 0, _msTexSize, _msTexSize);

    _meanshiftFramebuffer.bind();
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    // Set background color
    glClearColor(0, 0, 0, 1);
    // Clear fbo
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Enable additive blending
    glBlendFunc(GL_ONE, GL_ONE);

    // Bind shader
    _shaderDensityCompute.bind();

    // Set sigma uniform
    _shaderDensityCompute.uniform1f("sigma", _sigma);

    // Set advanced parameters uniform
    //>>>> float params[4] = { (_isScaleAvailable ? 1.0f / _maxScale : -1.0f), 0.0f, 0.0f, 0.0f };
    //>>>> _offscreenDensityShader.setParameter4fv(advancedParamsUniform, params);

    // Set gauss texture
    _gaussTexture.bind(0);
    _shaderDensityCompute.uniform1i("gaussSampler", 0);

    // Set the texture containing flags for the active state of each sample 
    //>>>> _offscreenDensityShader.uniform1i("activeSampleSampler", 1);
    //>>>> glActiveTexture(GL_TEXTURE1);
    //>>>> glBindTexture(GL_TEXTURE_1D, _activeSampleTexture);

    // Bind the vao
    glBindVertexArray(_vao);

    // Bind file ID buffer
    //if (_isFileIdxAvailable)
    //{
    //    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferSample);
    //    glVertexAttribIPointer(sampleIdAttribute, 1, GL_INT, 0, 0);
    //    glEnableVertexAttribArray(sampleIdAttribute);
    //}

    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, _numPoints);

    // Unbind vao
    glBindVertexArray(0);



    // Read pixels from framebuffer
    std::vector<float> kde(_msTexSize * _msTexSize * 3);

    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(0, 0, _msTexSize, _msTexSize, GL_RGB, GL_FLOAT, kde.data());

    // Calculate max value for normalization
    _maxKDE = -99999999.9f;
    for (int i = 0; i < kde.size(); i += 3) // only lookup red channel
    {
        _maxKDE = std::max(_maxKDE, kde[i]);
    }

    qDebug() << "	Max KDE Value = " << _maxKDE << ".\n";

    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());

    _needsDensityMapUpdate = false;
}


void DensityPlotWidget::drawGradientOffscreen()
{
    _meanshiftFramebuffer.bind();
    glDrawBuffer(GL_COLOR_ATTACHMENT1);

    glViewport(0, 0, _msTexSize, _msTexSize);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _shaderGradientCompute.bind();

    _densityTexture.bind(0);
    _shaderGradientCompute.uniform1i("densityTexture", 0);

    _shaderGradientCompute.uniform2f("renderParams", 1.0f / _maxKDE, 1.0f / 1000);

    drawFullscreenQuad();

    _shaderGradientCompute.release();

    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
}

void DensityPlotWidget::drawMeanshiftOffscreen()
{
    _meanshiftFramebuffer.bind();
    glDrawBuffer(GL_COLOR_ATTACHMENT2);

    glViewport(0, 0, _msTexSize, _msTexSize);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _shaderMeanshiftCompute.bind();

    _gradientTexture.bind(0);
    _shaderMeanshiftCompute.uniform1i("gradientTexture", 0);

    _shaderMeanshiftCompute.uniform4f("renderParams", 0.25f, 0.0f, 1.0f / _msTexSize, 1.0f / _msTexSize);

    drawFullscreenQuad();

    _shaderMeanshiftCompute.release();

    _meanShiftMapCPU.resize(_msTexSize * _msTexSize * 3);

    glReadBuffer(GL_COLOR_ATTACHMENT2);
    glReadPixels(0, 0, _msTexSize, _msTexSize, GL_RGB, GL_FLOAT, _meanShiftMapCPU.data());

    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
    qDebug() << "Drawing meanshift";
    //{
    //	double t = 0.0;
    //	{
    //		nut::ScopedTimer<double> timer(t);
    //
    //		for (int i = 0; i < 100; i++)
    //		{
    //			clusterOld();
    //		}
    //	}
    //	std::cout << "	Clustered 100 times in old mode in " << t << "\n";
    //	t = 0.0;
    //	{
    //		nut::ScopedTimer<double> timer(t);
    //
    //		for (int i = 0; i < 100; i++)
    //		{
    //			cluster();
    //		}
    //	}
    //	std::cout << "	Clustered 100 times in new mode in " << t << "\n";
    //}

    cluster();
}

void DensityPlotWidget::drawDensity()
{
    glViewport(0, 0, _windowSize.width(), _windowSize.height());

    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (_numPoints > 0) {
        _shaderDensityDraw.bind();

        _densityTexture.bind(0);
        _shaderDensityDraw.uniform1i("tex", 0);
        _shaderDensityDraw.uniform1f("norm", 1 / _maxKDE);

        drawFullscreenQuad();

        _shaderDensityDraw.release();
    }
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

void DensityPlotWidget::createSampleSelectionTextureBuffer()
{
    if (_activeSampleTexture == 0)
        glGenTextures(1, &_activeSampleTexture);

    glBindTexture(GL_TEXTURE_1D, _activeSampleTexture);

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    //glTexImage1D(GL_TEXTURE_1D, 0, GL_R32I, _localSampleSelectionForBuffer.size(), 0, GL_RED_INTEGER, GL_INT, _localSampleSelectionForBuffer.data());

    glBindTexture(GL_TEXTURE_1D, 0);
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

    _gaussTexture.destroy();

    glDeleteVertexArrays(1, &_vao);
    _positionBuffer.destroy();
}

} // namespace gui

} // namespace hdps
