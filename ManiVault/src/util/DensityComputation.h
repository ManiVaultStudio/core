// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "graphics/Bounds.h"
#include "graphics/BufferObject.h"
#include "graphics/Framebuffer.h"
#include "graphics/Shader.h"
#include "graphics/Texture.h"

#include "graphics/Vector2f.h"

#include <QOffscreenSurface>

namespace mv
{

class CORE_EXPORT GaussianTexture : public Texture2D
{
public:
    void generate();
};

class CORE_EXPORT DensityComputation : protected QOpenGLFunctions_3_3_Core
{
public:
    DensityComputation();
    ~DensityComputation() override;
    void init(QOpenGLContext* ctx);
    void cleanup();

    // Note: setData does not take the ownership of the vector specified by the argument.
    void setData(const std::vector<Vector2f>* data);
    void setWeights(const std::vector<float>* weights);
    void setBounds(float left, float right, float bottom, float top);
    void setSigma(float sigma);

    Texture2D& getDensityTexture() { return _densityTexture; }
    float getMaxDensity() const { return _maxKDE; }
    unsigned int getNumPoints() const { return _numPoints; }

    void compute();


    QSize getDensityTextureSize() const { return QSize(RESOLUTION, RESOLUTION); }

private:
    bool hasData() const;
    float calculateMaxKDE();

private:
    const unsigned int RESOLUTION       = 128;
    const float DEFAULT_SIGMA           = 0.15f;

    float _sigma                        = DEFAULT_SIGMA;
    float _maxKDE                       = -1;
    unsigned int _numPoints             = 0;
    Bounds _bounds                      = Bounds(-1, 1, 2, 2);

    ShaderProgram _shaderDensityCompute;
    Framebuffer _densityBuffer;
    Texture2D _densityTexture;
    GaussianTexture _gaussTexture;

    GLuint _vao;
    BufferObject _pointBuffer;
    BufferObject _weightsBuffer;
    const std::vector<Vector2f>* _points;
    const std::vector<float>* _weights;

    QOpenGLContext* _ctx;
    QOffscreenSurface _offscreenSurface;

    bool _initialized;
};

} // namespace mv
