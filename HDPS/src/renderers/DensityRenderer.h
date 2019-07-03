#pragma once

#include "Renderer.h"
#include "../SelectionListener.h"

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
            ~DensityRenderer() override;

            void setRenderMode(RenderMode renderMode);
            void setData(const std::vector<Vector2f>* data);
            void setBounds(float left, float right, float bottom, float top);
            void setSigma(const float sigma);
            /**
            * Loads a colormap from the given resource path and sets it
            * as the current colormap for the landscape view.
            *
            * @param colormap Resource path of a colormap
            */
            void setColormap(const QString colormap);

            void init() override;
            void resize(QSize renderSize) override;
            void render() override;
            void destroy() override;

            void onSelecting(Selection selection) override;
            void onSelection(Selection selection) override;

        private:
            void drawDensity();
            void drawLandscape();

            void drawFullscreenQuad();

        private:
            QSize _windowSize;

            bool _isSelecting = false;
            bool _hasColorMap = false;

            Selection _selection;
            ShaderProgram _shaderDensityDraw;
            ShaderProgram _shaderIsoDensityDraw;
            DensityComputation _densityComputation;
            Texture2D _colormap;
            

            RenderMode _renderMode;

            GLuint _quad;
        };

    } // namespace gui

} // namespace hdps
