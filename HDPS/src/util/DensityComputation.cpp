#include "DensityComputation.h"

#include "../graphics/Matrix3f.h"

namespace hdps
{

namespace
{
    float getMaxDimension(const std::vector<Vector2f>& points)
    {
        float maxDimension = 0;
        for (const Vector2f& point : points)
        {
            float len = point.sqrMagnitude();

            if (len > maxDimension) { maxDimension = len; }
        }
        return sqrt(maxDimension);
    }

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
}

void GaussianTexture::generate()
{
    create();

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

DensityComputation::DensityComputation()
    :
    _initialized(false),
    _needsDensityMapUpdate(true),
    _ctx(nullptr),
    _points(nullptr)
{

}

DensityComputation::~DensityComputation()
{
    delete _points;
}

void DensityComputation::init(QOpenGLContext* ctx)
{
    _ctx = ctx;

    initializeOpenGLFunctions();

    // Generate the gaussian rendering splat
    _gaussTexture.generate();

    // Build a VAO containing a quad and the instance-positions
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

    // Quad vertices and texture coordinates
    BufferObject quadBuffer;
    quadBuffer.create();
    quadBuffer.bind();
    quadBuffer.setData(quad);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));
    glEnableVertexAttribArray(1);

    // Positions of the points
    _pointBuffer.create();
    _pointBuffer.bind();
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribDivisor(2, 1);
    glEnableVertexAttribArray(2);

    // Load the density computation shader
    bool loaded = _shaderDensityCompute.loadShaderFromFile(":shaders/DensityCompute.vert", ":shaders/DensityCompute.frag");
    if (!loaded) {
        qDebug() << "Failed to load DensityCompute shader";
    }

    // Create the off-screen density framebuffer
    _densityBuffer.create();
    _densityBuffer.bind();

    // Create the texture on which we will render our splats
    _densityTexture.create();
    _densityTexture.bind();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, RESOLUTION, RESOLUTION, 0, GL_RGB, GL_FLOAT, nullptr);

    // Add the texture to the framebuffer and validate it
    _densityBuffer.addColorTexture(0, &_densityTexture);
    _densityBuffer.validate();

    _initialized = true;
}

void DensityComputation::cleanup()
{
    _initialized = false;

    // Destroy the computation shader
    _shaderDensityCompute.destroy();

    // Destroy the off-screen framebuffer
    _densityTexture.destroy();
    _densityBuffer.destroy();
    
    // Destroy the splat texture
    _gaussTexture.destroy();

    // Destroy the VAO
    glDeleteVertexArrays(1, &_vao);
    _pointBuffer.destroy();
    // FIXME: Other VBOs are not deleted
}

void DensityComputation::setData(const std::vector<Vector2f>* points)
{
    _points = points;
}

void DensityComputation::setBounds(float left, float right, float bottom, float top)
{
    _bounds.setLeft(left);
    _bounds.setRight(right);
    _bounds.setBottom(bottom);
    _bounds.setTop(top);
}

void DensityComputation::setSigma(float sigma)
{
    _sigma = sigma;
    
    compute();
}

void DensityComputation::compute()
{
    if (!_initialized) return;
    if (!hasData()) return;

    qDebug() << "Computing density";

    // Bind the OpenGL context to an off-screen surface to draw on
    _offscreenSurface.setFormat(_ctx->format());
    _offscreenSurface.setScreen(_ctx->screen());
    _offscreenSurface.create();
    _ctx->makeCurrent(&_offscreenSurface);

    _numPoints = _points->size();

    // Upload the points to the GPU
    _pointBuffer.bind();
    _pointBuffer.setData(*_points);

    // Bind the off-screen framebuffer
    _densityBuffer.bind();
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glViewport(0, 0, RESOLUTION, RESOLUTION);

    // Set background color and clear framebuffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    // Enable additive blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    // Bind shader
    _shaderDensityCompute.bind();

    // Set shader uniforms
    _shaderDensityCompute.uniform1f("sigma", _sigma);

    _gaussTexture.bind(0);
    _shaderDensityCompute.uniform1i("gaussSampler", 0);

    Matrix3f ortho = createProjectionMatrix(_bounds);
    _shaderDensityCompute.uniformMatrix3f("projMatrix", ortho);

    // Draw the splats
    glBindVertexArray(_vao);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, _numPoints);
    glBindVertexArray(0);

    _maxKDE = calculateMaxKDE();

    qDebug() << "	Max KDE Value = " << _maxKDE << ".\n";

    //_needsDensityMapUpdate = false;

    qDebug() << "Done computing density";
}

bool DensityComputation::hasData()
{
    return _points != nullptr && _points->size() > 0;
}

float DensityComputation::calculateMaxKDE()
{
    // Read back output pixels from the framebuffer
    std::vector<float> kde(RESOLUTION * RESOLUTION * 3);

    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(0, 0, RESOLUTION, RESOLUTION, GL_RGB, GL_FLOAT, kde.data());

    // Calculate max value for normalization
    float maxKDE = -99999999.9f;
    for (int i = 0; i < kde.size(); i += 3) // only lookup red channel
    {
        maxKDE = std::max(maxKDE, kde[i]);
    }

    return maxKDE;
}

}
