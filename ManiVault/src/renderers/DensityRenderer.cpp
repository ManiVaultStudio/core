// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DensityRenderer.h"

namespace mv
{
    namespace gui
    {

        DensityRenderer::DensityRenderer(RenderMode renderMode) :
            _renderMode(renderMode)
        {

        }

        DensityRenderer::~DensityRenderer()
        {
            // Delete objects
            _densityComputation.cleanup();
        }

        void DensityRenderer::setRenderMode(RenderMode renderMode)
        {
            _renderMode = renderMode;
        }

        // Points need to be passed as a pointer as we need to store them locally in order
        // to be able to recompute the densities when parameters change.
        void DensityRenderer::setData(const std::vector<Vector2f>* points)
        {
            _densityComputation.setData(points);
        }

        void DensityRenderer::setWeights(const std::vector<float>* weights)
        {
            _densityComputation.setWeights(weights);
        }

        void DensityRenderer::setBounds(const Bounds& bounds)
        {
            _densityComputation.setBounds(bounds.getLeft(), bounds.getRight(), bounds.getBottom(), bounds.getTop());
        }

        void DensityRenderer::setSigma(const float sigma)
        {
            _densityComputation.setSigma(sigma);
        }

        void DensityRenderer::computeDensity()
        {
            _densityComputation.compute();
        }

        float DensityRenderer::getMaxDensity() const
        {
            return _densityComputation.getMaxDensity();
        }

        mv::Vector3f DensityRenderer::getColorMapRange() const
        {
            return Vector3f(0.0f, _densityComputation.getMaxDensity(), _densityComputation.getMaxDensity());
        }

        void DensityRenderer::setColormap(const QImage& image)
        {
            _colormap.loadFromImage(image);
            _hasColorMap = true;
        }

        void DensityRenderer::init()
        {
            initializeOpenGLFunctions();

            // Create a simple VAO for full-screen quad rendering
            glGenVertexArrays(1, &_quad);

            // Load the necessary shaders for density drawing
            bool loaded = true;
            loaded &= _shaderDensityDraw.loadShaderFromFile(":shaders/Quad.vert", ":shaders/DensityDraw.frag");
            loaded &= _shaderIsoDensityDraw.loadShaderFromFile(":shaders/Quad.vert", ":shaders/IsoDensityDraw.frag");
            if (!loaded) {
                qDebug() << "Failed to load one of the Density shaders";
            }

            // Initialize the density computation
            _densityComputation.init(QOpenGLContext::currentContext());
        }

        void DensityRenderer::resize(QSize renderSize)
        {
            int w = renderSize.width();
            int h = renderSize.height();

            _windowSize.setWidth(w);
            _windowSize.setHeight(h);
        }

        void DensityRenderer::render()
        {
            glViewport(0, 0, _windowSize.width(), _windowSize.height());

            int w = _windowSize.width();
            int h = _windowSize.height();
            int size = w < h ? w : h;
            glViewport(w / 2 - size / 2, h / 2 - size / 2, size, size);

            // Draw density or isolines map
            switch (_renderMode) {
            case DENSITY: drawDensity(); break;
            case LANDSCAPE: drawLandscape(); break;
            }
        }

        void DensityRenderer::destroy()
        {
            _shaderDensityDraw.destroy();
            _shaderIsoDensityDraw.destroy();
            _densityComputation.cleanup();
            _colormap.destroy();

            glDeleteVertexArrays(1, &_quad);
        }

        void DensityRenderer::drawFullscreenQuad()
        {
            glBindVertexArray(_quad);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glBindVertexArray(0);
        }

        void DensityRenderer::drawDensity()
        {
            float maxDensity = _densityComputation.getMaxDensity();
            if (maxDensity <= 0) { return; }

            _shaderDensityDraw.bind();

            _densityComputation.getDensityTexture().bind(0);
            _shaderDensityDraw.uniform1i("tex", 0);
            _shaderDensityDraw.uniform1f("norm", 1 / maxDensity);

            drawFullscreenQuad();
        }

        void DensityRenderer::drawLandscape()
        {
            if (!_hasColorMap)
                return;

            float maxDensity = _densityComputation.getMaxDensity();
            if (maxDensity <= 0) { return; }

            _shaderIsoDensityDraw.bind();

            _densityComputation.getDensityTexture().bind(0);
            _shaderIsoDensityDraw.uniform1i("tex", 0);

            _shaderIsoDensityDraw.uniform2f("renderParams", 1.0f / maxDensity, 1.0f / _densityComputation.getNumPoints());
            _shaderIsoDensityDraw.uniform3f("colorMapRange", _colorMapRange);

            _colormap.bind(1);
            _shaderIsoDensityDraw.uniform1i("colormap", 1);

            drawFullscreenQuad();
        }

        void DensityRenderer::setColorMapRange(const float& min, const float& max)
        {
            _colorMapRange = Vector3f(min, max, max - min);
        }

    } // namespace gui

} // namespace mv
