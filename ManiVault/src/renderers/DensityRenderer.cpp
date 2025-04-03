// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DensityRenderer.h"

#ifdef _DEBUG
    //#define DENSITY_RENDERER_VERBOSE
#endif

namespace mv::gui
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

void DensityRenderer::resize(QSize renderSize)
{
    Renderer2D::resize(renderSize);

    updateQuad();
}

void DensityRenderer::setDataBounds(const QRectF& dataBounds)
{
    Renderer2D::setDataBounds(dataBounds);

    _densityComputation.setBounds(dataBounds.left(), dataBounds.right(), dataBounds.bottom(), dataBounds.top());

    updateQuad();
}

QRectF DensityRenderer::computeWorldBounds() const
{
    const auto textureSize  = static_cast<float>(_densityComputation.getDensityTextureSize().height());
    const auto marginX      = getNavigator().getZoomMarginScreen() * textureSize / (static_cast<float>(getRenderSize().height() - 2.f * getNavigator().getZoomMarginScreen()));
    const auto marginY      = getNavigator().getZoomMarginScreen() * textureSize / (static_cast<float>(getRenderSize().width() - 2.f * getNavigator().getZoomMarginScreen()));
    const auto margin       = std::max(marginX, marginY);
    const auto margins      = QMarginsF(margin, margin, margin, margin);

    return QRectF(QPointF(),_densityComputation.getDensityTextureSize()).marginsAdded(margins);
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
    return {
    	0.0f,
    	_densityComputation.getMaxDensity(),
    	_densityComputation.getMaxDensity()
    };
}

void DensityRenderer::setColormap(const QImage& image)
{
    _colormap.loadFromImage(image);
    _hasColorMap = true;
}

void DensityRenderer::init()
{
    initializeOpenGLFunctions();

    updateQuad();

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

void DensityRenderer::render()
{
    beginRender();
    {
        switch (_renderMode) {
			case DENSITY: {
				drawDensity();
				break;
			}

			case LANDSCAPE: {
				drawLandscape();
				break;
			}
        }
    }
    endRender();
}

void DensityRenderer::destroy()
{
    _shaderDensityDraw.destroy();
    _shaderIsoDensityDraw.destroy();
    _densityComputation.cleanup();
    _colormap.destroy();

    glDeleteVertexArrays(1, &_VAO);
    glDeleteVertexArrays(1, &_VBO);
    glDeleteVertexArrays(1, &_EBO);
}

void DensityRenderer::updateQuad()
{
    const auto textureSize  = _densityComputation.getDensityTextureSize().toSizeF();
    const auto width        = static_cast<float>(textureSize.width());
    const auto height       = static_cast<float>(textureSize.height());
	
    float vertices[] = {
        0.f,    0.f,        0.0f, 0.0f,
        width,  0.f,        1.0f, 0.0f,
        width,  height,     1.0f, 1.0f,
        0.f,    height,     0.0f, 1.0f
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);
    glGenBuffers(1, &_EBO);

    glBindVertexArray(_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinates attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void DensityRenderer::drawQuad()
{
    glBindVertexArray(_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void DensityRenderer::drawDensity()
{
#ifdef DENSITY_RENDERER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    const auto maxDensity = _densityComputation.getMaxDensity();

    if (maxDensity <= 0) {
        return;
    }

    _shaderDensityDraw.bind();
    {
        _densityComputation.getDensityTexture().bind(0);

        _shaderDensityDraw.uniformMatrix4f("mvp", getModelViewProjectionMatrix().data());
        _shaderDensityDraw.uniform1i("tex", 0);
        _shaderDensityDraw.uniform1f("norm", 1.0f / maxDensity);

        drawQuad();
    }
    _shaderDensityDraw.release();
}

void DensityRenderer::drawLandscape()
{
    if (!_hasColorMap)
        return;

#ifdef DENSITY_RENDERER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    const auto maxDensity = _densityComputation.getMaxDensity();

	if (maxDensity <= 0) {
		return;
	}

    _shaderIsoDensityDraw.bind();
    {
        _densityComputation.getDensityTexture().bind(0);

        _shaderIsoDensityDraw.uniformMatrix4f("mvp", getModelViewProjectionMatrix().data());
    	_shaderIsoDensityDraw.uniform1i("tex", 0);
        _shaderIsoDensityDraw.uniform2f("renderParams", 1.0f / maxDensity, 1.0f / _densityComputation.getNumPoints());
        _shaderIsoDensityDraw.uniform3f("colorMapRange", _colorMapRange);

        _colormap.bind(1);

        _shaderIsoDensityDraw.uniform1i("colormap", 1);

        drawQuad();
    }
    _shaderIsoDensityDraw.release();
}

void DensityRenderer::setColorMapRange(const float& min, const float& max)
{
    _colorMapRange = Vector3f(min, max, max - min);
}

}
