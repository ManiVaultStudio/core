#pragma once

#include "Renderer.h"
#include "SelectionListener.h"

#include "../graphics/Vector2f.h"
#include "../graphics/Texture.h"
#include "../graphics/Shader.h"

#include "../util/MeanShift.h"

#include <memory>

namespace hdps
{
    namespace gui
    {

        class DensityRenderer : public Renderer, public plugin::SelectionListener
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

            virtual void init();
            virtual void resize(QSize renderSize);
            virtual void render();
            virtual void destroy();

            virtual void onSelecting(Selection selection);
            virtual void onSelection(Selection selection);

        private:
            void drawDensity();
            void drawLandscape();

            void drawFullscreenQuad();

            QSize _windowSize;

            Selection _selection;
            bool _isSelecting = false;

            ShaderProgram _shaderDensityDraw;
            ShaderProgram _shaderIsoDensityDraw;
            DensityComputation _densityComputation;
            Texture2D _colorMap;

            RenderMode _renderMode;

            GLuint _quad;
        };

    } // namespace gui

} // namespace hdps
