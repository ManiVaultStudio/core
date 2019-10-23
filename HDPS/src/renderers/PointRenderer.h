#pragma once

#include "Renderer.h"
#include "../SelectionListener.h"

#include "../graphics/BufferObject.h"

#include "../graphics/Vector2f.h"
#include "../graphics/Vector3f.h"
#include "../graphics/Matrix3f.h"
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
            Color, Size, Outline
        };

        enum PointMode {
            Dot, Image
        };

        struct PointArrayObject : private QOpenGLFunctions_3_3_Core
        {
        public:
            GLuint _handle;

            BufferObject _positionBuffer;
            BufferObject _highlightBuffer;
            BufferObject _scalarBuffer;

            void init();
            void destroy();

        private:

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
            void setImageData(const std::vector<float>* data, unsigned int width, unsigned int height);
            void setColormap(const QString colormap);
            void setHighlight(const std::vector<char>& highlights);
            void setScalarProperty(const std::vector<float>& scalarProperty);
            void setScalarEffect(const PointEffect effect);
            void addScalarEffect(const PointEffect effect);
            void setBounds(float left, float right, float bottom, float top);
            void setPointSize(const float size);
            void setAlpha(const float alpha);
            void setPointScaling(PointScaling scalingMode);
            void setPointMode(PointMode pointMode);

            void init() override;
            void resize(QSize renderSize) override;
            void render() override;
            void destroy() override;

        private:
            PointArrayObject _gpuPoints;
            //GLuint _texArray;
            GLuint _texAtlas;

            ShaderProgram _shader;
            ShaderProgram _texShader;
            ShaderProgram _quadShader;
            ShaderProgram _selectionShader;

            unsigned int _numPoints = 0;
            const std::vector<Vector2f>* _positions;
            std::vector<char> _highlights;
            std::vector<float> _scalarProperty;

            QSize _windowSize;

            Matrix3f _ortho;

            /* Properties */
            PointSettings _pointSettings;
            PointEffect _pointEffect = PointEffect::Size;
            PointMode _pointMode = PointMode::Dot;

            Texture2D _colormap;

            QRectF       _bounds         = QRectF(-1, 1, 2, 2);
        };

    } // namespace gui

} // namespace hdps
