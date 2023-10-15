// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "Renderer.h"

#include "../graphics/Vector2f.h"
#include "../graphics/Vector3f.h"
#include "../graphics/Bounds.h"
#include "../graphics/Texture.h"
#include "../graphics/Shader.h"

#include "../util/MeanShift.h"

#include <memory>

namespace mv
{
    namespace gui
    {

        class DensityRenderer : public Renderer
        {

        public:
            enum RenderMode {
                DENSITY, LANDSCAPE
            };

            DensityRenderer(RenderMode renderMode);
            ~DensityRenderer() override;

            void setRenderMode(RenderMode renderMode);
            void setData(const std::vector<Vector2f>* data);
            void setBounds(const Bounds& bounds);
            void setSigma(const float sigma);
            void computeDensity();
            float getMaxDensity() const;
            Vector3f getColorMapRange() const;

            /**
             * Loads a colormap from an image and loads as 
             *the current colormap for the landscape view.
             * @param image Color map image
             */
            void setColormap(const QImage& image);

            void init() override;
            void resize(QSize renderSize) override;
            void render() override;
            void destroy() override;

            void setColorMapRange(const float& min, const float& max);

        private:
            void drawDensity();
            void drawLandscape();

            void drawFullscreenQuad();

        private:
            QSize _windowSize;

            bool _isSelecting = false;
            bool _hasColorMap = false;

            ShaderProgram _shaderDensityDraw;
            ShaderProgram _shaderIsoDensityDraw;
            DensityComputation _densityComputation;
            Texture2D _colormap;
            
            Vector3f _colorMapRange;

            RenderMode _renderMode;

            GLuint _quad;
        };

    } // namespace gui

} // namespace mv
