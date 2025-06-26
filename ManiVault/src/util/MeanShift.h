// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "DensityComputation.h"

#include "graphics/BufferObject.h"
#include "graphics/Framebuffer.h"
#include "graphics/Shader.h"
#include "graphics/Texture.h"
#include "graphics/Vector2f.h"

namespace mv
{

class CORE_EXPORT MeanShift : protected QOpenGLFunctions_3_3_Core
{
public:
    MeanShift() : _sigma(0.15f), _needsDensityMapUpdate(true), _quad(0) { }
    void init();
    void cleanup();

    void setData(const std::vector<Vector2f>* points);
    void setSigma(float sigma);

    void drawFullscreenQuad();

    void cluster(const std::vector<Vector2f>& points, std::vector<std::vector<unsigned int>>& clusters);
    bool equal(const Vector2f& p1, const Vector2f& p2, float epsilon);

    Texture2D& getGradientTexture();
    Texture2D& getMeanShiftTexture();

private:
    const unsigned int RESOLUTION = 256;

    DensityComputation densityComputation;

    void computeGradient();
    void computeMeanShift();

    ShaderProgram _shaderGradientCompute;
    ShaderProgram _shaderMeanshiftCompute;

    Framebuffer _meanshiftFramebuffer;
    Texture2D _gradientTexture;
    Texture2D _meanshiftTexture;

    const std::vector<Vector2f>* _points = nullptr;
    QRectF _bounds = QRectF(-1, 1, 2, 2);
    GLuint _quad;

    bool _needsDensityMapUpdate;
    float _sigma;

    std::vector<Vector2f> _meanshiftPixels;
    std::vector<Vector2f> _clusterPositions;
    std::vector<int> _clusterIds;
    std::vector<int> _clusterIdsOriginal;
};

} // namespace mv
