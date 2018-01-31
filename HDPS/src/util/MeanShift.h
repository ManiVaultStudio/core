#ifndef HDPS_MEAN_SHIFT_H
#define HDPS_MEAN_SHIFT_H

#include "../graphics/Framebuffer.h"
#include "../graphics/Texture.h"
#include "../graphics/Shader.h"
#include "../graphics/BufferObject.h"

#include "../graphics/Vector2f.h"

namespace hdps
{

class GaussianTexture : public Texture2D
{
public:
    void generate();
};

class MeanShift : protected QOpenGLFunctions_3_3_Core
{
public:
    MeanShift() : _sigma(0.15f), _msTexSize(512) { }
    void init();
    void cleanup();

    void setData(const std::vector<Vector2f>* points);

    void drawFullscreenQuad();
    void computeDensity();
    void computeGradient();
    void computeMeanShift();

    Texture2D& getDensityTexture();
    Texture2D& getGradientTexture();
    Texture2D& getMeanShiftTexture();
    float _maxKDE;
private:
    ShaderProgram _shaderDensityCompute;
    ShaderProgram _shaderGradientCompute;
    ShaderProgram _shaderMeanshiftCompute;

    GaussianTexture _gaussTexture;
    Framebuffer _meanshiftFramebuffer;
    Texture2D _densityTexture;
    Texture2D _gradientTexture;
    Texture2D _meanshiftTexture;

    BufferObject _positionBuffer;
    unsigned int _numPoints = 0;
    GLuint _vao;
    GLuint _quad;

    float _sigma;
    
    size_t _msTexSize;

    std::vector<float> _meanShiftMapCPU;
};

} // namespace hdps

#endif // HDPS_MEAN_SHIFT_H
