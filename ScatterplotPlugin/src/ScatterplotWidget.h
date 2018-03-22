#pragma once

#include "widgets/PointRenderer.h"
#include "widgets/DensityRenderer.h"

#include "SelectionListener.h"

#include "graphics/BufferObject.h"
#include "graphics/Vector2f.h"
#include "graphics/Vector3f.h"
#include "graphics/Matrix3f.h"
#include "graphics/Selection.h"
#include "graphics/Shader.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>

#include <QMouseEvent>
#include <memory>

namespace hdps
{
    namespace gui
    {

        class ScatterplotWidget : public QOpenGLWidget, QOpenGLFunctions_3_3_Core
        {
            Q_OBJECT
        public:
            enum RenderMode {
                SCATTERPLOT, DENSITY, LANDSCAPE
            };

            ScatterplotWidget();
            void setRenderMode(RenderMode renderMode);
            void setData(const std::vector<Vector2f>* data);
            void setColors(const std::vector<Vector3f>& data);
            void setPointSize(const float size);
            void setSelectionColor(const Vector3f selectionColor);
            void setAlpha(const float alpha);
            void setPointScaling(PointRenderer::PointScaling scalingMode);
            void setSigma(const float sigma);
            void addSelectionListener(const plugin::SelectionListener* listener);
        protected:
            void initializeGL()         Q_DECL_OVERRIDE;
            void resizeGL(int w, int h) Q_DECL_OVERRIDE;
            void paintGL()              Q_DECL_OVERRIDE;

            void mousePressEvent(QMouseEvent *event)   Q_DECL_OVERRIDE;
            void mouseMoveEvent(QMouseEvent *event)    Q_DECL_OVERRIDE;
            void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

            void onSelection(Selection selection);
            void cleanup();

        private:
            const Matrix3f toClipCoordinates = Matrix3f(2, 0, 0, 2, -1, -1);
            Matrix3f toNormalisedCoordinates;
            Matrix3f toIsotropicCoordinates;

            RenderMode _renderMode = SCATTERPLOT;

            PointRenderer _pointRenderer;
            DensityRenderer _densityRenderer;

            QSize _windowSize;

            unsigned int _numPoints = 0;
            const std::vector<Vector2f>* _positions;

            bool _selecting = false;
            Selection _selection;
            std::vector<const plugin::SelectionListener*> _selectionListeners;
        };

    } // namespace gui

} // namespace hdps
