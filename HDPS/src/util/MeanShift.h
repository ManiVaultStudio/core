#ifndef HDPS_MEAN_SHIFT_H
#define HDPS_MEAN_SHIFT_H

#include "DensityComputation.h"

#include "../graphics/Framebuffer.h"
#include "../graphics/Texture.h"
#include "../graphics/Shader.h"
#include "../graphics/BufferObject.h"

#include "../graphics/Vector2f.h"

namespace hdps
{

class MeanShift : protected QOpenGLFunctions_3_3_Core
{
public:
    MeanShift() : _sigma(0.15f), _needsDensityMapUpdate(true) { }
    void init();
    void cleanup();

    void setData(const std::vector<Vector2f>* points);

    void drawFullscreenQuad();

    void cluster(const std::vector<Vector2f>& points, std::vector<std::vector<unsigned int>>& clusters);
    bool equal(const std::vector<float> &p1, const std::vector<float> &p2, float epsilon);

    Texture2D& getGradientTexture();
    Texture2D& getMeanShiftTexture();

private:
    const unsigned int RESOLUTION = 512;

    DensityComputation densityComputation;

    void computeGradient();
    void computeMeanShift();

    ShaderProgram _shaderGradientCompute;
    ShaderProgram _shaderMeanshiftCompute;

    Framebuffer _meanshiftFramebuffer;
    Texture2D _gradientTexture;
    Texture2D _meanshiftTexture;

    const std::vector<Vector2f>* _points;
    GLuint _quad;

    bool _needsDensityMapUpdate;
    float _sigma;

    std::vector<float> _meanShiftMapCPU;
    std::vector<Vector2f> _clusterPositions;
    std::vector<int> _clusterIds;
    std::vector<int> _clusterIdsOriginal;
};

} // namespace hdps

#endif // HDPS_MEAN_SHIFT_H
