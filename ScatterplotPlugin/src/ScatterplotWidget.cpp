#include "ScatterplotWidget.h"

#include <vector>

#include <QSize>
#include <QDebug>

namespace hdps
{
    namespace gui
    {

        ScatterplotWidget::ScatterplotWidget()
            :
            _densityRenderer(DensityRenderer::RenderMode::DENSITY)
        {

        }

        void ScatterplotWidget::setRenderMode(RenderMode renderMode)
        {
            _renderMode = renderMode;

            update();
        }

        // Positions need to be passed as a pointer as we need to store them locally in order
        // to be able to find the subset of data that's part of a selection. If passed
        // by reference then we can upload the data to the GPU, but not store it in the widget.
        void ScatterplotWidget::setData(const std::vector<Vector2f>* points)
        {
            _pointRenderer.setData(points);
            _densityRenderer.setData(points);

            update();
        }

        void ScatterplotWidget::setColors(const std::vector<Vector3f>& colors)
        {
            _pointRenderer.setColors(colors);

            update();
        }

        void ScatterplotWidget::setPointSize(const float size)
        {
            _pointRenderer.setPointSize(size);

            update();
        }

        void ScatterplotWidget::setSelectionColor(const Vector3f selectionColor)
        {
            _pointRenderer.setSelectionColor(selectionColor);

            update();
        }

        void ScatterplotWidget::setAlpha(const float alpha)
        {
            _pointRenderer.setAlpha(alpha);
        }

        void ScatterplotWidget::setPointScaling(PointRenderer::PointScaling scalingMode)
        {
            _pointRenderer.setPointScaling(scalingMode);
        }

        void ScatterplotWidget::setSigma(const float sigma)
        {
            _densityRenderer.setSigma(sigma);

            update();
        }

        void ScatterplotWidget::addSelectionListener(const plugin::SelectionListener* listener)
        {
            _selectionListeners.push_back(listener);
        }

        void ScatterplotWidget::initializeGL()
        {
            qDebug() << "Initializing scatterplot";

            initializeOpenGLFunctions();

            connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &ScatterplotWidget::cleanup);

            _pointRenderer.init();
            _densityRenderer.init(context());
        }

        void ScatterplotWidget::resizeGL(int w, int h)
        {
            qDebug() << "Resizing scatterplot";
            _pointRenderer.resize(QSize(w, h));
            _densityRenderer.resize(w, h);
            qDebug() << "Done resizing scatterplot";
        }

        void ScatterplotWidget::paintGL()
        {
            // Bind the framebuffer belonging to the widget
            glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());

            switch (_renderMode)
            {
            case SCATTERPLOT: _pointRenderer.render(); break;
            case DENSITY:
            {
                _densityRenderer.setRenderMode(DensityRenderer::DENSITY);
                _densityRenderer.render();
                break;
            }
            case LANDSCAPE:
            {
                _densityRenderer.setRenderMode(DensityRenderer::LANDSCAPE);
                _densityRenderer.render();
                break;
            }
            }
        }

        void ScatterplotWidget::mousePressEvent(QMouseEvent *event)
        {
            _selecting = true;

            Vector2f point = toNormalisedCoordinates * Vector2f(event->x(), _windowSize.height() - event->y());
            _selection.setStart(point);
        }

        void ScatterplotWidget::mouseMoveEvent(QMouseEvent *event)
        {
            if (!_selecting) return;

            Vector2f point = toNormalisedCoordinates * Vector2f(event->x(), _windowSize.height() - event->y());
            _selection.setEnd(point);

            update();
        }

        void ScatterplotWidget::mouseReleaseEvent(QMouseEvent *event)
        {
            _selecting = false;

            Vector2f point = toNormalisedCoordinates * Vector2f(event->x(), _windowSize.height() - event->y());
            _selection.setEnd(point);

            onSelection(_selection);
        }

        void ScatterplotWidget::onSelection(Selection selection)
        {
            update();

            std::vector<unsigned int> indices;
            for (unsigned int i = 0; i < _numPoints; i++)
            {
                Vector2f point = (*_positions)[i];
                point.x *= _windowSize.width() / _windowSize.height();

                if (selection.contains(point))
                    indices.push_back(i);
            }

            for (const plugin::SelectionListener* listener : _selectionListeners)
                listener->onSelection(indices);
        }

        void ScatterplotWidget::cleanup()
        {
            qDebug() << "Deleting scatterplot widget, performing clean up...";
            makeCurrent();

            _pointRenderer.destroy();

            _densityRenderer.terminate();
        }

    } // namespace gui

} // namespace hdps
