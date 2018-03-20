#pragma once

#include "../graphics/Vector2f.h"
#include "../graphics/Texture.h"
#include "../graphics/Shader.h"

#include "../util/MeanShift.h"

#include <memory>

namespace hdps
{
    namespace gui
    {

        class DensityRenderer : protected QOpenGLFunctions_3_3_Core
        {

        public:
            enum RenderMode {
                DENSITY, LANDSCAPE
            };

            DensityRenderer(RenderMode renderMode);
            ~DensityRenderer();

            void setRenderMode(RenderMode renderMode);
            void setData(const std::vector<Vector2f>* data);
            void setSigma(const float sigma);

            void init(QOpenGLContext* context);
            void resize(int w, int h);
            void render();
            void terminate();

        private:
            void drawDensity();
            void drawLandscape();

            void drawFullscreenQuad();

            QSize _windowSize;

            ShaderProgram _shaderDensityDraw;
            ShaderProgram _shaderIsoDensityDraw;
            DensityComputation _densityComputation;
            Texture2D _colorMap;

            RenderMode _renderMode;

            GLuint _quad;
        };

    } // namespace gui

} // namespace hdps
