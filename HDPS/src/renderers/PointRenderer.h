#pragma once

#include "Renderer.h"

#include "../graphics/BufferObject.h"
#include "../graphics/Vector2f.h"
#include "../graphics/Vector3f.h"
#include "../graphics/Matrix3f.h"
#include "../graphics/Bounds.h"
#include "../graphics/Texture.h"

#include <QRectF>

namespace hdps
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

        struct PointArrayObject : private QOpenGLFunctions_3_3_Core
        {
        public:
            GLuint _handle;

            BufferObject _positionBuffer;
            BufferObject _highlightBuffer;
            BufferObject _colorScalarBuffer;
            BufferObject _sizeScalarBuffer;
            BufferObject _opacityScalarBuffer;
            BufferObject _colorBuffer;

            PointArrayObject() : _handle(0), _colorScalarsRange(0, 1, 1) {}
            void init();
            void setPositions(const std::vector<Vector2f>& positions);
            void setHighlights(const std::vector<char>& highlights);
            void setScalars(const std::vector<float>& scalars);
            void setSizeScalars(const std::vector<float>& scalars);
            void setOpacityScalars(const std::vector<float>& scalars);
            void setColors(const std::vector<Vector3f>& colors);

            void enableAttribute(uint index, bool enable);

            bool hasHighlights() const { return !_highlights.empty(); }
            bool hasColorScalars() const { return !_colorScalars.empty(); }
            bool hasSizeScalars() const { return !_sizeScalars.empty(); }
            bool hasOpacityScalars() const { return !_opacityScalars.empty(); }
            bool hasColors() const { return !_colors.empty(); }

            Vector3f getColorMapRange() const {
                return _colorScalarsRange;
            }

            void setColorMapRange(const hdps::Vector3f& colorMapRange) {
                _colorScalarsRange = colorMapRange;
            }

            void draw();
            void destroy();

        private:

            /** Vertex array indices */
            const uint ATTRIBUTE_VERTICES           = 0;
            const uint ATTRIBUTE_POSITIONS          = 1;
            const uint ATTRIBUTE_HIGHLIGHTS         = 2;
            const uint ATTRIBUTE_COLORS             = 4;
            const uint ATTRIBUTE_SCALARS_COLOR      = 3;
            const uint ATTRIBUTE_SCALARS_SIZE       = 5;
            const uint ATTRIBUTE_SCALARS_OPACITY    = 6;

            /* Point attributes */
            std::vector<Vector2f>   _positions;
            std::vector<Vector3f>   _colors;
            std::vector<char>       _highlights;
            
            /** Scalar channels */
            std::vector<float>  _colorScalars;      /** Point color scalar channel */
            std::vector<float>  _sizeScalars;       /** Point size scalar channel */
            std::vector<float>  _opacityScalars;    /** Point opacity scalar channel */

            /** Scalar ranges */
            Vector3f    _colorScalarsRange;     /** Scalar range of the point color scalars */

            bool _dirtyPositions        = false;
            bool _dirtyHighlights       = false;
            bool _dirtyColorScalars     = false;
            bool _dirtySizeScalars      = false;
            bool _dirtyOpacityScalars   = false;
            bool _dirtyColors           = false;
        };

        struct PointSettings
        {
            // Constants
            const float         DEFAULT_POINT_SIZE      = 15;
            const float         DEFAULT_ALPHA_VALUE     = 0.5f;
            const PointScaling  DEFAULT_POINT_SCALING   = PointScaling::Relative;

            PointScaling        _scalingMode            = DEFAULT_POINT_SCALING;
            float               _pointSize              = DEFAULT_POINT_SIZE;
            float               _alpha                  = DEFAULT_ALPHA_VALUE;
        };

        class PointRenderer : public Renderer
        {
        public:
            void setData(const std::vector<Vector2f>& points);
            void setHighlights(const std::vector<char>& highlights, const std::int32_t& numSelectedPoints);
            void setColorChannelScalars(const std::vector<float>& scalars);
            void setSizeChannelScalars(const std::vector<float>& scalars);
            void setOpacityChannelScalars(const std::vector<float>& scalars);
            void setColors(const std::vector<Vector3f>& colors);

            void setScalarEffect(const PointEffect effect);
            void setColormap(const QImage& image);
            void setBounds(const Bounds& bounds);
            void setPointSize(const float size);
            void setAlpha(const float alpha);
            void setPointScaling(PointScaling scalingMode);
            
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
            void setSelectionHaloEnabled(float selectionHaloEnabled);

            void init() override;
            void resize(QSize renderSize) override;
            void render() override;
            void destroy() override;

            Vector3f getColorMapRange() const;
            void setColorMapRange(const float& min, const float& max);

        private:
            /* Point properties */
            PointSettings _pointSettings;
            PointEffect   _pointEffect = PointEffect::Size;
            
            PointSelectionDisplayMode   _selectionDisplayMode               = PointSelectionDisplayMode::Outline;
            Vector3f                    _selectionOutlineColor              = Vector3f(0, 0, 1);
            bool                        _selectionOutlineOverrideColor      = true;
            float                       _selectionOutlineScale              = 1.75f;
            float                       _selectionOutlineOpacity            = 0.5f;
            bool                        _selectionHaloEnabled               = false;

            /* Window properties */
            QSize                       _windowSize;

            /* Rendering variables */
            ShaderProgram               _shader;

            PointArrayObject            _gpuPoints;
            Texture2D                   _colormap;

            Matrix3f                    _orthoM;
            Bounds                      _bounds                             = Bounds(-1, 1, -1, 1);

            std::int32_t                _numSelectedPoints                  = 0;     /** Number of selected (highlighted points) */
        };

    } // namespace gui

} // namespace hdps
