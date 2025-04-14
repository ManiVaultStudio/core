// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "Renderer2D.h"

#include "graphics/Bounds.h"
#include "graphics/BufferObject.h"
#include "graphics/Matrix3f.h"
#include "graphics/Texture.h"
#include "graphics/Vector2f.h"
#include "graphics/Vector3f.h"

namespace mv
{
    namespace gui
    {
        enum PointScaling {
            Absolute, Relative
        };

        enum PointEffect {
            None, Color, Size, Outline, Color2D
        };

        enum class PointSelectionDisplayMode {
            Outline, Override
        };

        struct CORE_EXPORT PointArrayObject : private QOpenGLFunctions_3_3_Core
        {
        public:
            GLuint _handle;

            BufferObject _positionBuffer;
            BufferObject _highlightBuffer;
            BufferObject _focusHighlightsBuffer;
            BufferObject _colorScalarBuffer;
            BufferObject _sizeScalarBuffer;
            BufferObject _opacityScalarBuffer;
            BufferObject _colorBuffer;

            PointArrayObject() : QOpenGLFunctions_3_3_Core(), _handle(0), _colorScalarsRange(0, 1, 1) {}
            void init();
            void setPositions(const std::vector<Vector2f>& positions);
            void setHighlights(const std::vector<char>& highlights);
            void setFocusHighlights(const std::vector<char>& focusHighlights);
            void setScalars(const std::vector<float>& scalars, bool adjustColorMapRange);
            void setSizeScalars(const std::vector<float>& scalars);
            void setOpacityScalars(const std::vector<float>& scalars);
            void setColors(const std::vector<Vector3f>& colors);

            const std::vector<Vector2f>& getPositions() const { return _positions; }
            const std::vector<char>& getHighlights() const { return _highlights; }
            const std::vector<char>& getFocusHighlights() const { return _focusHighlights; }
            const std::vector<float>& getScalars() const { return _colorScalars; }
            const std::vector<float>& getSizeScalars() const { return _sizeScalars; }
            const std::vector<float>& getOpacityScalars() const { return _opacityScalars; }
            const std::vector<Vector3f>& getColors() const { return _colors; }

            void enableAttribute(uint index, bool enable);

            bool hasHighlights() const { return !_highlights.empty(); }
            bool hasFocusHighlights() const { return !_focusHighlights.empty(); }
            bool hasColorScalars() const { return !_colorScalars.empty(); }
            bool hasSizeScalars() const { return !_sizeScalars.empty(); }
            bool hasOpacityScalars() const { return !_opacityScalars.empty(); }
            bool hasColors() const { return !_colors.empty(); }

            Vector3f getColorMapRange() const {
                return _colorScalarsRange;
            }

            void setColorMapRange(const mv::Vector3f& colorMapRange) {
                _colorScalarsRange = colorMapRange;
            }

            void draw();

            void destroy();

        private:

            /** Vertex array indices */
            const uint ATTRIBUTE_VERTICES           = 0;
            const uint ATTRIBUTE_POSITIONS          = 1;
            const uint ATTRIBUTE_HIGHLIGHTS         = 2;
            const uint ATTRIBUTE_FOCUS_HIGHLIGHTS   = 3;
            const uint ATTRIBUTE_COLORS             = 5;
            const uint ATTRIBUTE_SCALARS_COLOR      = 4;
            const uint ATTRIBUTE_SCALARS_SIZE       = 6;
            const uint ATTRIBUTE_SCALARS_OPACITY    = 7;

            /* Point attributes */
            std::vector<Vector2f>   _positions;
            std::vector<Vector3f>   _colors;
            std::vector<char>       _highlights;
            std::vector<char>       _focusHighlights;
            
            /** Scalar channels */
            std::vector<float>  _colorScalars;      /** Point color scalar channel */
            std::vector<float>  _sizeScalars;       /** Point size scalar channel */
            std::vector<float>  _opacityScalars;    /** Point opacity scalar channel */

            /** Scalar ranges */
            Vector3f    _colorScalarsRange;     /** Scalar range of the point color scalars */

            bool _dirtyPositions        = false;
            bool _dirtyHighlights       = false;
            bool _dirtyFocusHighlights  = false;
            bool _dirtyColorScalars     = false;
            bool _dirtySizeScalars      = false;
            bool _dirtyOpacityScalars   = false;
            bool _dirtyColors           = false;

            BufferObject _quadBufferObject;
        };

        struct CORE_EXPORT PointSettings
        {
            // Constants
            const float         DEFAULT_POINT_SIZE      = 15;
            const float         DEFAULT_ALPHA_VALUE     = 0.5f;
            const PointScaling  DEFAULT_POINT_SCALING   = PointScaling::Relative;

            PointScaling        _scalingMode            = DEFAULT_POINT_SCALING;
            float               _pointSize              = DEFAULT_POINT_SIZE;
            float               _alpha                  = DEFAULT_ALPHA_VALUE;
        };

        class CORE_EXPORT PointRenderer : public Renderer2D
        {
        public:
            using Renderer2D::Renderer2D;

            PointRenderer() = default;
            PointRenderer(QWidget* sourceWidget, QObject* parent = nullptr);

            /**
             * Set data bounds to \p dataBounds
             * @param dataBounds Data bounds
             */
            void setDataBounds(const QRectF& dataBounds) override;

            /** Update the world bounds */
            QRectF computeWorldBounds() const override;

            void setData(const std::vector<Vector2f>& points);
            void setHighlights(const std::vector<char>& highlights, const std::int32_t& numSelectedPoints);
            void setFocusHighlights(const std::vector<char>& focusHighlights, const std::int32_t& numberOfFocusHighlights);
            void setColorChannelScalars(const std::vector<float>& scalars, bool adjustColorMapRange = true);
            void setSizeChannelScalars(const std::vector<float>& scalars);
            void setOpacityChannelScalars(const std::vector<float>& scalars);
            void setColors(const std::vector<Vector3f>& colors);

            PointEffect getScalarEffect() const;
            void setScalarEffect(const PointEffect effect);

            void setColormap(const QImage& image);

            const PointArrayObject& getGpuPoints() const;
            std::int32_t getNumSelectedPoints() const;

            const PointSettings& getPointSettings() const;
            void setPointSize(const float size);
            void setAlpha(const float alpha);
            void setPointScaling(PointScaling scalingMode);

            void initView();

        public: // Selection visualization

            PointSelectionDisplayMode getSelectionDisplayMode() const;
            void setSelectionDisplayMode(PointSelectionDisplayMode selectionDisplayMode);

            Vector3f getSelectionOutlineColor() const;
            void setSelectionOutlineColor(Vector3f color);

            bool getSelectionOutlineOverrideColor() const;
            void setSelectionOutlineOverrideColor(float selectionOutlineOverrideColor);

            float getSelectionOutlineScale() const;
            void setSelectionOutlineScale(float selectionOutlineScale);

            float getSelectionOutlineOpacity() const;
            void setSelectionOutlineOpacity(float selectionOutlineOpacity);

            bool getSelectionHaloEnabled() const;
            void setSelectionHaloEnabled(bool selectionHaloEnabled);

            bool getRandomizedDepthEnabled() const;
            void setRandomizedDepthEnabled(bool randomizedDepth);

            void init() override;
            void render() override;
            void destroy() override;

            Vector3f getColorMapRange() const;
            void setColorMapRange(const float& min, const float& max);

        private:
            /* Point properties */
            PointSettings               _pointSettings = {};
            PointEffect                 _pointEffect = PointEffect::Size;

            /** Selection visualization */
            PointSelectionDisplayMode   _selectionDisplayMode               = PointSelectionDisplayMode::Outline;
            Vector3f                    _selectionOutlineColor              = Vector3f(0, 0, 1);
            bool                        _selectionOutlineOverrideColor      = true;
            float                       _selectionOutlineScale              = 1.75f;
            float                       _selectionOutlineOpacity            = 0.5f;
            bool                        _selectionHaloEnabled               = false;

            /* Depth control */
            bool                        _randomizedDepthEnabled             = true;

            /* Rendering variables */
            ShaderProgram               _shader = {};
            PointArrayObject            _gpuPoints = {};
            Texture2D                   _colormap = {};                                                     /** 2D colormap, sets point color based on point position */
            std::int32_t                _numSelectedPoints                  = 0;                            /** Number of selected (highlighted points) */
            std::int32_t                _numberOfFocusHighlights            = 0;                            /** Number of focus highlights */
        };

    } // namespace gui

} // namespace mv
