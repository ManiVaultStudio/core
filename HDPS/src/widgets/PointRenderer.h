#pragma once

#include "Renderer.h"
#include "SelectionListener.h"

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

        struct PointSettings
        {
            // Constants
            const float        DEFAULT_POINT_SIZE = 15;
            const Vector3f     DEFAULT_SELECTION_COLOR = Vector3f(1.0f, 0.5f, 0.0f);
            const float        DEFAULT_ALPHA_VALUE = 0.5f;
            const PointScaling DEFAULT_POINT_SCALING = PointScaling::Relative;

            PointScaling _scalingMode = DEFAULT_POINT_SCALING;
            Vector3f     _selectionColor = DEFAULT_SELECTION_COLOR;
            float        _pointSize = DEFAULT_POINT_SIZE;
            float        _alpha = DEFAULT_ALPHA_VALUE;
        };

        class PointRenderer : public Renderer, public plugin::SelectionListener
        {
        public:
            

            void setData(const std::vector<Vector2f>* points);
            void setColors(const std::vector<Vector3f>& data);
            void setHighlight(const std::vector<char>& highlights);
            void setBounds(float left, float right, float bottom, float top);
            void setPointSize(const float size);
            void setSelectionColor(const Vector3f selectionColor);
            void setAlpha(const float alpha);
            void setPointScaling(PointScaling scalingMode);

            virtual void init();
            virtual void resize(QSize renderSize);
            virtual void render();
            virtual void destroy();

            virtual void onSelecting(Selection selection);
            virtual void onSelection(Selection selection);

        private:
            const Matrix3f toClipCoordinates = Matrix3f(2, 0, 0, 2, -1, -1);
            Matrix3f toIsotropicCoordinates;

            GLuint _vao;

            BufferObject _positionBuffer;
            BufferObject _colorBuffer;
            BufferObject _highlightBuffer;
            ShaderProgram _shader;
            ShaderProgram _selectionShader;

            Selection _selection;
            bool _isSelecting = false;

            unsigned int _numPoints = 0;
            const std::vector<Vector2f>* _positions;
            std::vector<Vector3f> _colors;
            std::vector<char> _highlights;

            QSize _windowSize;

            Matrix3f _ortho;

            /* Properties */
            PointSettings _pointSettings;

            QRectF       _bounds         = QRectF(-1, 1, 2, 2);
        };

    } // namespace gui

} // namespace hdps
