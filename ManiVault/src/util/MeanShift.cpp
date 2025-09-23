// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "MeanShift.h"

#include "graphics/Matrix3f.h"

#include <algorithm>

#include <QImage>
#include <QDebug>
#include <math.h>
#include <float.h>
//#define MEANSHIFT_IMAGE_DEBUG

namespace mv
{

Matrix3f createProjectionMatrix(QRectF bounds)
{
    Matrix3f m;
    m.setIdentity();
    m[0] = 2 / (bounds.right() - bounds.left());
    m[4] = 2 / (bounds.top() - bounds.bottom());
    m[6] = -((bounds.right() + bounds.left()) / (bounds.right() - bounds.left()));
    m[7] = -((bounds.top() + bounds.bottom()) / (bounds.top() - bounds.bottom()));
    return m;
}
void MeanShift::init()
{
    initializeOpenGLFunctions();

    glClearColor(1, 1, 1, 1);
    
    densityComputation.init(QOpenGLContext::currentContext());

    glGenVertexArrays(1, &_quad);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    bool loaded = _shaderGradientCompute.loadShaderFromFile(":shaders/GradientCompute.vert", ":shaders/GradientCompute.frag");
    if (!loaded) {
        qDebug() << "Failed to load GradientCompute shader";
    }

    loaded = _shaderMeanshiftCompute.loadShaderFromFile(":shaders/MeanshiftCompute.vert", ":shaders/MeanshiftCompute.frag");
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

QRectF getDataBounds(const std::vector<Vector2f>& points)
{
    float maxDimension = -FLT_MAX;
    for (const Vector2f& point : points)
    {
        maxDimension = std::max(std::abs(point.x), maxDimension);
        maxDimension = std::max(std::abs(point.y), maxDimension);
    }

    // setting a square to make sure that the gaussian is not squished
    QRectF bounds;
    bounds.setLeft(-maxDimension);
    bounds.setRight(maxDimension);
    bounds.setBottom(-maxDimension);
    bounds.setTop(maxDimension);

    return bounds;
}

void MeanShift::setData(const std::vector<Vector2f>* points)
{
    QRectF bounds = getDataBounds(*points);
    
    _points = points;
    _bounds = bounds;
    densityComputation.setData(points);
    densityComputation.setBounds(bounds.left(), bounds.right(), bounds.bottom(), bounds.top());
    _needsDensityMapUpdate = true;
}

void MeanShift::setSigma(float sigma)
{
    _sigma = sigma;
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

    _shaderGradientCompute.uniform4f("renderParams", 1.0f / densityComputation.getMaxDensity(), 1.0f / 100000, 1.0f / RESOLUTION, 1.0f);

    drawFullscreenQuad();

    _shaderGradientCompute.release();

#ifdef MEANSHIFT_IMAGE_DEBUG
    std::vector<float> gradientValues;
    gradientValues.resize(RESOLUTION * RESOLUTION * 3);

    glReadBuffer(GL_COLOR_ATTACHMENT1);
    glReadPixels(0, 0, RESOLUTION, RESOLUTION, GL_RGB, GL_FLOAT, gradientValues.data());

    QImage gradients(RESOLUTION, RESOLUTION, QImage::Format::Format_RGB32);
    float maxL = 0.0f;
    for (int j = 0; j < RESOLUTION; ++j)
    {
        for (int i = 0; i < RESOLUTION; ++i)
        {
            int idx = j * RESOLUTION + i;

            Vector2f p(gradientValues[idx * 3], gradientValues[idx * 3 + 1]);
            float l = p.length();
            
            maxL = std::max(l, maxL);
        }
    }

    qDebug() << "Max gradient magnitude = " << maxL << "\n";

    for (int j = 0; j < RESOLUTION; ++j)
    {
        for (int i = 0; i < RESOLUTION; ++i)
        {
            int idx = j * RESOLUTION + i;

            Vector2f p(gradientValues[idx * 3], gradientValues[idx * 3 + 1]);
            float l = p.length();
            p = p / l;

            // angle between (1,0) and p
            float a = acos(p.x) / 3.14159265f * 0.5;

            if (p.y < 0.0f)
            {
                a = 1.0f - a;
            }
            

            QColor col;
            col.setHsvF(a, l / maxL, 1.0);
            
            gradients.setPixelColor(i, RESOLUTION - j - 1, col);

        }
    }
    gradients.save("gradient.png");
    qDebug() << "Saved gradients";
#endif
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
//    qDebug() << "Drawing meanshift";
    _shaderMeanshiftCompute.release();

    _meanshiftPixels.resize(RESOLUTION * RESOLUTION);

    glReadBuffer(GL_COLOR_ATTACHMENT2);
    glReadPixels(0, 0, RESOLUTION, RESOLUTION, GL_RG, GL_FLOAT, _meanshiftPixels.data());

#ifdef MEANSHIFT_IMAGE_DEBUG
    QImage centers(RESOLUTION, RESOLUTION, QImage::Format::Format_RGB32);
    for (int j = 0; j < RESOLUTION; ++j)
    {
        for (int i = 0; i < RESOLUTION; ++i)
        {
            int idx = j * RESOLUTION + i;
            centers.setPixel(i, RESOLUTION - j - 1, qRgb(_meanshiftPixels[idx].x * 255, _meanshiftPixels[idx].y * 255, 0));
        }
    }
    centers.save("meanshift_centers.png");
    Matrix3f ortho = createProjectionMatrix(_bounds);
    for (int i = 0; i < _points->size(); i++) {
        // Calculate the coordinate of the pixel center on the texture
        Vector2f p = (ortho * (*_points)[i]);

        const Vector2f point = p * 0.5 + 0.5;
        int x = (int)(point.x * (RESOLUTION - 1) + 0.5);
        int y = (int)(point.y * (RESOLUTION - 1) + 0.5);

        centers.setPixel(x, y, qRgb(255, 0, 0));
    }

    centers.save("meanshift_centers_points.png");
    qDebug() << "Saved meanshift";
#endif
}

void MeanShift::cluster(const std::vector<Vector2f>& points, std::vector<std::vector<unsigned int>>& clusters)
{
    if (points.size() == 0) return;

    densityComputation.setSigma(_sigma);
    densityComputation.compute();
    computeGradient();
    computeMeanShift();

    // Resize clusterID arrays to equal number of pixels
    _clusterIds.resize(RESOLUTION * RESOLUTION);
    _clusterIdsOriginal.resize(RESOLUTION * RESOLUTION);
    // Stores centers of all clusters that are found in meanshift segmentation
    std::vector<Vector2f> clusterCenters;

    // we take a distance of 2 pixels as maximum to assume points ended in the same peak
    float epsilon = 2.0f/RESOLUTION;// 0.05f;
    // For every pixel in Mean Shift Map
    for (int i = 0; i < _meanshiftPixels.size(); i++) {
        // Set center to red and green component of pixel
        Vector2f center = _meanshiftPixels[i];

        // If center equals 0, set clusterID for this pixel to -1
        if (center.sqrMagnitude() < 0.0001)
        {
            _clusterIdsOriginal[i] = -1;
            continue;
        }

        // If the center already exist in clusterCenters set clusterID to that cluster
        bool clusterFound = false;
        int clusterId = -1;
        for (int c = 0; c < clusterCenters.size(); c++)
        {
            if (equal(center, clusterCenters[c], epsilon))
            {
                clusterFound = true;
                clusterId = c;
                break;
            }
        }
        
        // If clusterID was not found already, push the new center into clusterCenters
        if (!clusterFound)
        {
            clusterCenters.push_back(center);
            // Set the current pixel in clusterIdsOriginal to the latest cluster index
            _clusterIdsOriginal[i] = static_cast<int>(clusterCenters.size() - 1);
        }
        else
        {
            // Set the current pixel in clusterIdsOriginal to the previously found clusterID
            _clusterIdsOriginal[i] = clusterId;
        }
    }

#ifdef MEANSHIFT_IMAGE_DEBUG
    for (int i = 0; i < clusterCenters.size(); i++) {
        qDebug() << "Cluster center: " << clusterCenters[i].x << " " << clusterCenters[i].y;
    }
#endif //MEANSHIFT_IMAGE_DEBUG

    // Create a vector with the same size as the number of clusters, and set all IDs to -1
    std::vector<int> activeIds(clusterCenters.size(), -1);

    Matrix3f ortho = createProjectionMatrix(_bounds);

    int runningIdx = 0;
    _clusterPositions.clear();
    // For every point
    for (int i = 0; i < _points->size(); i++) {
        // Calculate the coordinate of the pixel center on the texture
        Vector2f p = (ortho * (*_points)[i]);

        const Vector2f point = p * 0.5 + 0.5;
        int x = (int)(point.x * (RESOLUTION - 1) + 0.5);
        int y = (int)(point.y * (RESOLUTION - 1) + 0.5);

        // Calculate index into clusterID array this pixel belongs to
        int pixelIndex = (x + y * RESOLUTION);
        // Get the clusterID
        int cId = _clusterIdsOriginal[pixelIndex];
        // If the clusterID is 0 or more and cluster is not active
        if (cId >= 0 && activeIds[cId] < 0) {
            // Get the cluster center
            Vector2f center = clusterCenters[cId];
            // Store the cluster center as the position of the cluster
            _clusterPositions.push_back(center);

            activeIds[cId] = runningIdx++;
        }
    }

#pragma omp parallel for
    // For every assigned clusterID, set it to the corresponding active ID and store it in clusterIds
    for (int i = 0; i < _clusterIdsOriginal.size(); i++) {
        if (_clusterIdsOriginal[i] >= 0){ _clusterIdsOriginal[i] = activeIds[_clusterIdsOriginal[i]]; }
        _clusterIds[i] = _clusterIdsOriginal[i];
    }

    // Check if clusters contain their own cluster center.
    // If not it is likely that the center is just a variation of an existing cluster and those should be merged
#pragma omp parallel for
    for (int i = 0; i < _clusterIds.size(); i++) {

        Vector2f currentCenter = _meanshiftPixels[i];
        int x = static_cast<int>(currentCenter.x * (RESOLUTION - 1) + 0.5);
        int y = static_cast<int>(currentCenter.y * (RESOLUTION - 1) + 0.5);
        int centerIdx = (x + y * RESOLUTION);

        if (centerIdx < 0 || centerIdx >= _clusterIds.size())
            continue;

        if (_clusterIds[i] != _clusterIds[centerIdx] && _clusterIds[i] >= 0 && _clusterIds[centerIdx] >= 0)
        {
            //qDebug() << "Assigned from cluster " << _clusterIds[i] << " to " << _clusterIds[centerIdx] << "index: " << i << ", pos(" << currentCenter.x << ", " << currentCenter.y << ")";
            _clusterIds[i] = _clusterIds[centerIdx];
        }
    }
    
    // Divide points into their corresponding clusters
    //qDebug() << "Matrix: " << ortho[0] << "," << ortho[1] << "," << ortho[2] << "," << ortho[3] << "," << ortho[4] << "," << ortho[5] << "," << ortho[6] << "," << ortho[7] << "," << ortho[8];
    clusters.resize(runningIdx);
    for (int i = 0; i < _points->size(); i++) {
        Vector2f p = (ortho * (*_points)[i]);

        const Vector2f point = p * 0.5 + 0.5;
        int x = (int)(point.x * (RESOLUTION - 1) + 0.5);
        int y = (int)(point.y * (RESOLUTION - 1) + 0.5);

        int pixelIndex = (x + y * RESOLUTION);
        //_clusterIds[pixelIndex] = qRgb(255, 0, 0);

        int cId = _clusterIds[pixelIndex];
        if (cId >= 0) { clusters[cId].push_back(i); }
    }

    //qDebug() << "Final clusters size (including empty clusters): " << clusters.size();

    clusters.erase(std::remove_if(clusters.begin(), clusters.end(),
        [](const auto& pointIDs) { return pointIDs.empty(); }),
        clusters.end());

    qDebug() << "Final clusters size: " << clusters.size();

#ifdef MEANSHIFT_IMAGE_DEBUG
    QImage clustersImg(RESOLUTION, RESOLUTION, QImage::Format::Format_RGB32);
    float scale = 255.0 / clusterCenters.size();
    for (int j = 0; j < RESOLUTION; ++j)
    {
        for (int i = 0; i < RESOLUTION; ++i)
        {
            int idx = j * RESOLUTION + i;
            clustersImg.setPixel(i, RESOLUTION - j - 1, qRgb(_clusterIds[idx] * scale, _clusterIds[idx] * scale, _clusterIds[idx] * scale));
        }
    }
    clustersImg.save("meanshift_clusters.png");
    qDebug() << "Saved image of clusters";
#endif
}

bool MeanShift::equal(const Vector2f& p1, const Vector2f& p2, float epsilon)
{
    return fabs(p1.x - p2.x) < epsilon && fabs(p1.y - p2.y < epsilon);
}

Texture2D& MeanShift::getGradientTexture()
{
    return _gradientTexture;
}

Texture2D& MeanShift::getMeanShiftTexture()
{
    return _meanshiftTexture;
}

std::uint32_t MeanShift::getResolution() const
{
    return RESOLUTION;
}

} // namespace mv
