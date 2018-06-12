#pragma once

#include "Renderer.h"
#include "../SelectionListener.h"

#include "../graphics/BufferObject.h"

#include "../graphics/Vector2f.h"
#include "../graphics/Vector3f.h"
#include "../graphics/Matrix3f.h"

#include <QRectF>

namespace hdps
{
    namespace gui
    {
        enum PointScaling {
            Absolute, Relative
        };

        enum PointEffect {
            Colour, Size, Outline
        };

        struct PointArrayObject : private QOpenGLFunctions_3_3_Core
        {
            GLuint _handle;

            BufferObject _positionBuffer;
            BufferObject _colorBuffer;
            BufferObject _highlightBuffer;
            BufferObject _scalarBuffer;

            void init();
            void destroy();
        };

        struct PointSettings
        {
            // Constants
            const float        DEFAULT_POINT_SIZE = 15;
            const float        DEFAULT_ALPHA_VALUE = 0.5f;
            const PointScaling DEFAULT_POINT_SCALING = PointScaling::Relative;

            PointScaling _scalingMode = DEFAULT_POINT_SCALING;
            float        _pointSize = DEFAULT_POINT_SIZE;
            float        _alpha = DEFAULT_ALPHA_VALUE;
        };

        class PointRenderer : public Renderer
        {
        public:
            void setData(const std::vector<Vector2f>* points);
            void setColors(const std::vector<Vector3f>& data);
            void setHighlight(const std::vector<char>& highlights);
            void setScalarProperty(const std::vector<float>& scalarProperty);
            void setBounds(float left, float right, float bottom, float top);
            void setPointSize(const float size);
            void setAlpha(const float alpha);
            void setPointScaling(PointScaling scalingMode);

            virtual void init() override;
            virtual void resize(QSize renderSize) override;
            virtual void render() override;
            virtual void destroy() override;

        private:
            PointArrayObject _gpuPoints;

            ShaderProgram _shader;
            ShaderProgram _selectionShader;

            unsigned int _numPoints = 0;
            const std::vector<Vector2f>* _positions;
            std::vector<Vector3f> _colors;
            std::vector<char> _highlights;
            std::vector<float> _scalarProperty;

            QSize _windowSize;

            Matrix3f _ortho;

            /* Properties */
            PointSettings _pointSettings;
            PointEffect _pointEffect = PointEffect::Size;

            QRectF       _bounds         = QRectF(-1, 1, 2, 2);
        };

    } // namespace gui

} // namespace hdps
