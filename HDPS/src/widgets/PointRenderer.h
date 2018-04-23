#pragma once

#include "Renderer.h"

#include "../graphics/BufferObject.h"

#include "../graphics/Vector2f.h"
#include "../graphics/Vector3f.h"
#include "../graphics/Matrix3f.h"

#include "../graphics/Selection.h"

namespace hdps
{
    namespace gui
    {
        class PointRenderer : protected Renderer
        {
        public:
            enum PointScaling {
                Absolute, Relative
            };

            void setData(const std::vector<Vector2f>* points);
            void setColors(const std::vector<Vector3f>& data);
            void setBounds(float left, float right, float bottom, float top);
            void setPointSize(const float size);
            void setSelectionColor(const Vector3f selectionColor);
            void setAlpha(const float alpha);
            void setPointScaling(PointScaling scalingMode);

            virtual void init();
            virtual void resize(QSize renderSize);
            virtual void render();
            virtual void destroy();

            void onSelection(Selection selection);

        private:
            const Matrix3f toClipCoordinates = Matrix3f(2, 0, 0, 2, -1, -1);
            Matrix3f toNormalisedCoordinates;
            Matrix3f toIsotropicCoordinates;

            const float        DEFAULT_POINT_SIZE = 15;
            const Vector3f     DEFAULT_SELECTION_COLOR = Vector3f(1.0f, 0.5f, 0.0f);
            const float        DEFAULT_ALPHA_VALUE = 0.5f;
            const PointScaling DEFAULT_POINT_SCALING = PointScaling::Relative;

            GLuint _vao;

            BufferObject _positionBuffer;
            BufferObject _colorBuffer;
            ShaderProgram _shader;

            unsigned int _numPoints = 0;
            const std::vector<Vector2f>* _positions;
            std::vector<Vector3f> _colors;

            QSize _windowSize;
            PointScaling _scalingMode = DEFAULT_POINT_SCALING;
            Vector3f _selectionColor = DEFAULT_SELECTION_COLOR;
            float _pointSize = DEFAULT_POINT_SIZE;
            float _alpha = DEFAULT_ALPHA_VALUE;

            ShaderProgram _selectionShader;
            Selection _selection;
        };

    } // namespace gui

} // namespace hdps
