#include "PointRenderer.h"

#include <QDebug>

namespace hdps
{
    namespace gui
    {
        namespace
        {
            Matrix3f createProjectionMatrix(QRectF bounds)
            {
                Matrix3f m;
                m.setIdentity();
                m[0] = 2 / (bounds.right() - bounds.left());
                m[4] = 2 / (bounds.top() - bounds.bottom());
                m[6] = -((bounds.right() + bounds.left()) / (bounds.right() - bounds.left()));
                m[7] = -((bounds.top() + bounds.bottom()) / (bounds.top() - bounds.bottom()));
                return m;
            }
        }

        void PointArrayObject::init()
        {
            initializeOpenGLFunctions();

            glGenVertexArrays(1, &_handle);
            glBindVertexArray(_handle);

            std::vector<float> vertices(
            {
                -1, -1,
                1, -1,
                -1, 1,
                -1, 1,
                1, -1,
                1, 1
            }
            );

            BufferObject quad;
            quad.create();
            quad.bind();
            quad.setData(vertices);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(0);

            _positionBuffer.create();
            _positionBuffer.bind();
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
            glVertexAttribDivisor(1, 1);
            glEnableVertexAttribArray(1);

            _highlightBuffer.create();
            _highlightBuffer.bind();
            glVertexAttribIPointer(2, 1, GL_UNSIGNED_BYTE, 0, 0);
            glVertexAttribDivisor(2, 1);
            glEnableVertexAttribArray(2);

            _scalarBuffer.create();
            _scalarBuffer.bind();
            glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, 0);
            glVertexAttribDivisor(3, 1);
            glEnableVertexAttribArray(3);
        }

        void PointArrayObject::destroy()
        {
            glDeleteVertexArrays(1, &_handle);
            _positionBuffer.destroy();
        }

        // Positions need to be passed as a pointer as we need to store them locally in order
        // to be able to find the subset of data that's part of a selection. If passed
        // by reference then we can upload the data to the GPU, but not store it in the widget.
        void PointRenderer::setData(const std::vector<Vector2f>* points)
        {
            _numPoints = (unsigned int) points->size();
            _positions = points;
            _highlights.clear();
            _highlights.resize(_numPoints, 0);
            _scalarProperty.clear();
            _scalarProperty.resize(_numPoints, 1);

            glBindVertexArray(_gpuPoints._handle);
            _gpuPoints._positionBuffer.bind();
            _gpuPoints._positionBuffer.setData(*points);
            _gpuPoints._highlightBuffer.bind();
            _gpuPoints._highlightBuffer.setData(_highlights);
            _gpuPoints._scalarBuffer.bind();
            _gpuPoints._scalarBuffer.setData(_scalarProperty);
            glBindVertexArray(0);
        }

        void PointRenderer::setColormap(const QString colormap)
        {
            _colormap.loadFromFile(colormap);
        }

        void PointRenderer::setHighlight(const std::vector<char>& highlights)
        {
            _highlights = highlights;

            glBindVertexArray(_gpuPoints._handle);
            _gpuPoints._highlightBuffer.bind();
            _gpuPoints._highlightBuffer.setData(highlights);
            glBindVertexArray(0);
        }

        void PointRenderer::setScalarProperty(const std::vector<float>& scalarProperty)
        {
            _scalarProperty = scalarProperty;

            glBindVertexArray(_gpuPoints._handle);
            _gpuPoints._scalarBuffer.bind();
            _gpuPoints._scalarBuffer.setData(scalarProperty);
            glBindVertexArray(0);
        }

        void PointRenderer::setScalarEffect(const PointEffect effect)
        {
            _pointEffect = effect;
        }

        void PointRenderer::setBounds(float left, float right, float bottom, float top)
        {
            _bounds.setLeft(left);
            _bounds.setRight(right);
            _bounds.setBottom(bottom);
            _bounds.setTop(top);
        }

        void PointRenderer::setPointSize(const float size)
        {
            _pointSettings._pointSize = size;
        }

        void PointRenderer::setAlpha(const float alpha)
        {
            _pointSettings._alpha = alpha;
            _pointSettings._alpha = _pointSettings._alpha > 1 ? 1 : _pointSettings._alpha;
            _pointSettings._alpha = _pointSettings._alpha < 0 ? 0 : _pointSettings._alpha;
        }

        void PointRenderer::setPointScaling(PointScaling scalingMode)
        {
            _pointSettings._scalingMode = scalingMode;
        }

        void PointRenderer::init()
        {
            qDebug() << "Initializing scatterplot";

            initializeOpenGLFunctions();

            _gpuPoints.init();

            if (_numPoints > 0)
            {
                glBindVertexArray(_gpuPoints._handle);
                _gpuPoints._positionBuffer.bind();
                _gpuPoints._positionBuffer.setData(*_positions);
                _gpuPoints._highlightBuffer.bind();
                _gpuPoints._highlightBuffer.setData(_highlights);
                _gpuPoints._scalarBuffer.bind();
                _gpuPoints._scalarBuffer.setData(_scalarProperty);
            }

            bool loaded = true;
            loaded &= _shader.loadShaderFromFile(":shaders/PointPlot.vert", ":shaders/PointPlot.frag");

            if (!loaded) {
                qDebug() << "Failed to load one of the Scatterplot shaders";
            }
        }

        void PointRenderer::resize(QSize renderSize)
        {
            int w = renderSize.width();
            int h = renderSize.height();

            _windowSize.setWidth(w);
            _windowSize.setHeight(h);
        }

        void PointRenderer::render()
        {
            int w = _windowSize.width();
            int h = _windowSize.height();
            int size = w < h ? w : h;
            glViewport(w / 2 - size / 2, h / 2 - size / 2, size, size);

            // World to clip transformation
            _ortho = createProjectionMatrix(_bounds);

            _shader.bind();

            switch (_pointSettings._scalingMode) {
            case Relative: _shader.uniform1f("pointSize", _pointSettings._pointSize); break;
            case Absolute: _shader.uniform1f("pointSize", _pointSettings._pointSize / size); break;
            }

            _shader.uniformMatrix3f("projMatrix", _ortho);
            _shader.uniform1f("alpha", _pointSettings._alpha);
            _shader.uniform1i("scalarEffect", _pointEffect);

            if (_pointEffect == PointEffect::Color) {
                _colormap.bind(0);
                _shader.uniform1i("colormap", 0);
            }

            glBindVertexArray(_gpuPoints._handle);
            glDrawArraysInstanced(GL_TRIANGLES, 0, 6, _numPoints);
            glBindVertexArray(0);
        }

        void PointRenderer::destroy()
        {
            _gpuPoints.destroy();
        }

    } // namespace gui

} // namespace hdps
