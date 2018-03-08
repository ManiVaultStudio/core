#include "MeanShift.h"

#include <QImage>

namespace hdps
{

void MeanShift::init()
{
    initializeOpenGLFunctions();

    glClearColor(1, 1, 1, 1);
    qDebug() << "Initializing density plot";

    glGenVertexArrays(1, &_quad);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    bool loaded = _shaderGradientCompute.loadShaderFromFile(":shaders/Quad.vert", ":shaders/GradientCompute.frag");
    if (!loaded) {
        qDebug() << "Failed to load GradientCompute shader";
    }

    loaded = _shaderMeanshiftCompute.loadShaderFromFile(":shaders/Quad.vert", ":shaders/MeanshiftCompute.frag");
    if (!loaded) {
        qDebug() << "Failed to load MeanshiftCompute shader";
    }

    _meanshiftFramebuffer.create();
    _meanshiftFramebuffer.bind();

    _gradientTexture.create();
    _gradientTexture.bind();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, RESOLUTION, RESOLUTION, 0, GL_RGB, GL_FLOAT, NULL);

    _meanshiftFramebuffer.addColorTexture(1, &_gradientTexture);
    _meanshiftFramebuffer.validate();

    _meanshiftTexture.create();
    _meanshiftTexture.bind();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, RESOLUTION, RESOLUTION, 0, GL_RGB, GL_FLOAT, NULL);

    _meanshiftFramebuffer.addColorTexture(2, &_meanshiftTexture);
    _meanshiftFramebuffer.validate();
}

void MeanShift::cleanup()
{

}

void MeanShift::setData(const std::vector<Vector2f>* points)
{
    //_numPoints = (unsigned int) points->size();
    _points = points;
    _needsDensityMapUpdate = true;
}

void MeanShift::drawFullscreenQuad()
{
    glBindVertexArray(_quad);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

void MeanShift::computeGradient()
{
    _meanshiftFramebuffer.bind();
    glDrawBuffer(GL_COLOR_ATTACHMENT1);

    glViewport(0, 0, RESOLUTION, RESOLUTION);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _shaderGradientCompute.bind();

    densityComputation.getDensityTexture().bind(0);
    _shaderGradientCompute.uniform1i("densityTexture", 0);

    _shaderGradientCompute.uniform2f("renderParams", 1.0f / densityComputation.getMaxDensity(), 1.0f / 100000);

    drawFullscreenQuad();

    _shaderGradientCompute.release();

    //glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
}

void MeanShift::computeMeanShift()
{
    _meanshiftFramebuffer.bind();
    glDrawBuffer(GL_COLOR_ATTACHMENT2);

    glViewport(0, 0, RESOLUTION, RESOLUTION);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _shaderMeanshiftCompute.bind();

    _gradientTexture.bind(0);
    _shaderMeanshiftCompute.uniform1i("gradientTexture", 0);

    _shaderMeanshiftCompute.uniform4f("renderParams", 0.25f, densityComputation.getMaxDensity(), 1.0f / RESOLUTION, 1.0f / RESOLUTION);

    drawFullscreenQuad();

    _shaderMeanshiftCompute.release();

    _meanShiftMapCPU.resize(RESOLUTION * RESOLUTION * 3);

    glReadBuffer(GL_COLOR_ATTACHMENT2);
    glReadPixels(0, 0, RESOLUTION, RESOLUTION, GL_RGB, GL_FLOAT, _meanShiftMapCPU.data());

    // DEBUG ======================================================================
    QImage pluto(RESOLUTION, RESOLUTION, QImage::Format::Format_ARGB32);
    for (int j = 0; j < RESOLUTION; ++j)
    {
        for (int i = 0; i < RESOLUTION; ++i)
    	{
            int idx = j * RESOLUTION + i;
    		pluto.setPixel(i, j, qRgb(_meanShiftMapCPU[idx * 3] * 255, _meanShiftMapCPU[idx * 3 + 1] * 255, _meanShiftMapCPU[idx * 3 + 2] * 255));
    	}
    }
    pluto.save("meanshift.png");
    // DEBUG ======================================================================


    //for (int i = 0; i < _meanShiftMapCPU.size(); i++) {
    //    qDebug() << "Value: " << _meanShiftMapCPU[i] << std::endl;
    //}
    qDebug() << "Computing meanshift";
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

}

void MeanShift::cluster(const std::vector<Vector2f>& points, std::vector<std::vector<unsigned int>>& clusters)
{
    if (points.size() == 0) return;

    densityComputation.compute();
    computeGradient();
    computeMeanShift();

    // Resize clusterID arrays to equal number of pixels
    _clusterIds.resize(RESOLUTION * RESOLUTION);
    _clusterIdsOriginal.resize(RESOLUTION * RESOLUTION);
    // Stores centers of all clusters that are found in meanshift segmentation
    std::vector< std::vector< float > > clusterCenters;

    float epsilon = (float)RESOLUTION / (128 * 256);
    // For every pixel in Mean Shift Map
    for (int i = 0; i < _meanShiftMapCPU.size(); i += 3) {
        // Set center to red and green component of pixel
        std::vector< float > center = { _meanShiftMapCPU[i], _meanShiftMapCPU[i + 1] };

        // If either of the center components are 0 or lower, set clusterID for this pixel to -1
        if (center[0] < 0.00001 && center[1] < 0.00001)
        {
            _clusterIdsOriginal[i / 3] = -1;
            continue;
        }

        // If the center already exist in clusterCenters set clusterID to that cluster
        int clusterId = -1;
        for (int c = 0; c < clusterCenters.size(); c++)
        {
            if (equal(center, clusterCenters[c], epsilon))
            {
                clusterId = c;
                break;
            }
        }
        
        // If clusterID was not found already, push the new center into clusterCenters
        if (clusterId < 0)
        {
            qDebug() << "Pushing: " << center[0] << " " << center[1];
            clusterCenters.push_back(center);
            // Set the current pixel in clusterIdsOriginal to the latest cluster index
            _clusterIdsOriginal[i / 3] = static_cast<int>(clusterCenters.size() - 1);
        }
        else
        {
            // Set the current pixel in clusterIdsOriginal to the previously found clusterID
            _clusterIdsOriginal[i / 3] = clusterId;
        }

        //std::cout << center[0] << ", " << center[1];
    }

    for (int i = 0; i < clusterCenters.size(); i++) {
        qDebug() << "Cluster center: " << clusterCenters[i][0] << " " << clusterCenters[i][1];
    }

    // DEBUG ======================================================================
    QImage pluto(RESOLUTION, RESOLUTION, QImage::Format::Format_ARGB32);
    float scale = 255.0 / clusterCenters.size();
    for (int j = 0; j < RESOLUTION; ++j)
    {
        for (int i = 0; i < RESOLUTION; ++i)
        {
            int idx = j * RESOLUTION + i;
            pluto.setPixel(i, j, qRgb(_clusterIds[idx] * scale, _clusterIds[idx] * scale, _clusterIds[idx] * scale));
        }
    }
    pluto.save("meanshift_clusters.png");
    qDebug() << "Saved image of clusters";
    // DEBUG ======================================================================

    //std::vector< std::vector< std::pair <int, int> > > mcvClusters(clusterCenters.size());

    // Create a vector with the same size as the number of clusters, and set all IDs to -1
    std::vector<int> activeIds(clusterCenters.size(), -1);

    int runningIdx = 0;
    _clusterPositions.clear();
    // For every point
    for (int i = 0; i < _points->size(); i++) {
        // Calculate the coordinate of the pixel center on the texture
        const Vector2f& point = (*_points)[i] * 0.5 + 0.5;
        int x = (int)(point.x * (RESOLUTION - 1) + 0.5);
        int y = (int)(point.y * (RESOLUTION - 1) + 0.5);

        // Calculate index into clusterID array this pixel belongs to
        int pixelIndex = (x + y * RESOLUTION);
        // Get the clusterID
        int cId = _clusterIdsOriginal[pixelIndex];
        // If the clusterID is 0 or more and cluster is not active
        //std::cout << "ClusterID: " << cId << " ActiveIds size: " << activeIds.size() << std::endl;
        if (cId >= 0 && activeIds[cId] < 0) {
            // Get the cluster center
            std::vector< float > center = { clusterCenters[cId] };
            // Store the cluster center as the position of the cluster
            _clusterPositions.push_back(Vector2f(center[0], center[1]));

            activeIds[cId] = runningIdx++;
        }
    }

#pragma omp parallel for
    // For every assigned clusterID, set it to the corresponding active ID and store it in clusterIds
    for (int i = 0; i < _clusterIdsOriginal.size(); i++) {
        if (_clusterIdsOriginal[i] >= 0){ _clusterIdsOriginal[i] = activeIds[_clusterIdsOriginal[i]]; }
        _clusterIds[i] = _clusterIdsOriginal[i];
    }

    // Divide points into their corresponding clusters
    clusters.resize(runningIdx);
    for (int i = 0; i < _points->size(); i++) {
        const Vector2f& point = (*_points)[i] * 0.5 + 0.5;
        int x = (int)(point.x * (RESOLUTION - 1) + 0.5);
        int y = (int)(point.y * (RESOLUTION - 1) + 0.5);

        int pixelIndex = (x + y * RESOLUTION);

        int cId = _clusterIdsOriginal[pixelIndex];
        if (cId >= 0) { clusters[cId].push_back(i); }
    }

    //>>>_meanShiftClusters = _analysis->updateClusters(&clusterIdxs, "Density Clusters");

    //>>>refreshClusterBuffers();

    //>>>_clustersNeedRefresh = false;
}

bool MeanShift::equal(const std::vector<float> &p1, const std::vector<float> &p2, float epsilon)
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

Texture2D& MeanShift::getGradientTexture()
{
    return _gradientTexture;
}

Texture2D& MeanShift::getMeanShiftTexture()
{
    return _meanshiftTexture;
}

} // namespace hdps
