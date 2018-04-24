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
            addSelectionListener(&_selectionRenderer);
            addSelectionListener(&_pointRenderer);
        }

        void ScatterplotWidget::setRenderMode(RenderMode renderMode)
        {
            _renderMode = renderMode;

            update();
        }

        // Positions need to be passed as a pointer as we need to store them locally in order
        // to be able to find the subset of data that's part of a selection. If passed
        // by reference then we can upload the data to the GPU, but not store it in the widget.
        void ScatterplotWidget::setData(const std::vector<Vector2f>* points, const QRectF bounds)
        {

            float width = fabs(bounds.width());
            float height = fabs(bounds.height());
            float size = width > height ? width : height;
            Vector2f center((bounds.left() + bounds.right()) / 2, (bounds.bottom() + bounds.top()) / 2);
            QRectF uniformBounds;
            uniformBounds.setLeft(center.x - size / 2);
            uniformBounds.setRight(center.x + size / 2);
            uniformBounds.setBottom(center.y - size / 2);
            uniformBounds.setTop(center.y + size / 2);
            _dataBounds = uniformBounds;

            _pointRenderer.setBounds(_dataBounds.left(), _dataBounds.right(), _dataBounds.bottom(), _dataBounds.top());
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

        void ScatterplotWidget::addSelectionListener(plugin::SelectionListener* listener)
        {
            _selectionListeners.push_back(listener);
        }

        float lerp(float v0, float v1, float t) {
            return (1 - t) * v0 + t * v1;
        }

        Selection ScatterplotWidget::getSelection()
        {
            Vector2f bottomLeft = _selection.bottomLeft();
            Vector2f topRight = _selection.topRight();
            bottomLeft = toIsotropicCoordinates * bottomLeft;
            topRight = toIsotropicCoordinates * topRight;

            Vector2f bottomLeftData(lerp(_dataBounds.left(), _dataBounds.right(), bottomLeft.x), lerp(_dataBounds.bottom(), _dataBounds.top(), bottomLeft.y));
            Vector2f topRightData(lerp(_dataBounds.left(), _dataBounds.right(), topRight.x), lerp(_dataBounds.bottom(), _dataBounds.top(), topRight.y));
            
            return Selection(bottomLeftData, topRightData);
        }

        void ScatterplotWidget::initializeGL()
        {
            qDebug() << "Initializing scatterplot";

            initializeOpenGLFunctions();

            connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &ScatterplotWidget::cleanup);

            _pointRenderer.init();
            _densityRenderer.init();
            _selectionRenderer.init();
        }

        void ScatterplotWidget::resizeGL(int w, int h)
        {
            qDebug() << "Resizing scatterplot";
            _windowSize.setWidth(w);
            _windowSize.setHeight(h);

            _pointRenderer.resize(QSize(w, h));
            _densityRenderer.resize(QSize(w, h));
            _selectionRenderer.resize(QSize(w, h));

            toNormalisedCoordinates = Matrix3f(1.0f / w, 0, 0, 1.0f / h, 0, 0);

            int size = w < h ? w : h;

            float wAspect = (float)w / size;
            float hAspect = (float)h / size;
            float wDiff = ((wAspect - 1) / 2.0);
            float hDiff = ((hAspect - 1) / 2.0);

            toIsotropicCoordinates = Matrix3f(wAspect, 0, 0, hAspect, -wDiff, -hDiff);
            qDebug() << "Done resizing scatterplot";
        }

        void ScatterplotWidget::paintGL()
        {
            // Bind the framebuffer belonging to the widget
            glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());

            // Clear the widget to the background color
            glClearColor(1, 1, 1, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Reset the blending function
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
            _selectionRenderer.render();
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

            onSelecting(_selection);

            update();
        }

        void ScatterplotWidget::mouseReleaseEvent(QMouseEvent *event)
        {
            _selecting = false;

            Vector2f point = toNormalisedCoordinates * Vector2f(event->x(), _windowSize.height() - event->y());
            _selection.setEnd(point);

            onSelection(_selection);

            update();
        }

        void ScatterplotWidget::onSelecting(Selection selection)
        {
            _selection.set(selection.getStart(), selection.getEnd());

            for (plugin::SelectionListener* listener : _selectionListeners)
                listener->onSelecting(selection);

            update();
        }

        void ScatterplotWidget::onSelection(Selection selection)
        {
            _selection.set(selection.getStart(), selection.getEnd());

            for (plugin::SelectionListener* listener : _selectionListeners)
                listener->onSelection(_selection);

            update();
        }

        void ScatterplotWidget::cleanup()
        {
            qDebug() << "Deleting scatterplot widget, performing clean up...";
            makeCurrent();

            _pointRenderer.destroy();
            _densityRenderer.destroy();
            _selectionRenderer.destroy();
        }

    } // namespace gui

} // namespace hdps
