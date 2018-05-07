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

        // Positions need to be passed as a pointer as we need to store them locally in order
        // to be able to find the subset of data that's part of a selection. If passed
        // by reference then we can upload the data to the GPU, but not store it in the widget.
        void PointRenderer::setData(const std::vector<Vector2f>* points)
        {
            _numPoints = (unsigned int) points->size();
            _positions = points;
            _colors.clear();
            _colors.resize(_numPoints, Vector3f(0.5f, 0.5f, 0.5f));
            _highlights.clear();
            _highlights.resize(_numPoints, 0);
            _scalarProperty.clear();
            _scalarProperty.resize(_numPoints, 1);

            glBindVertexArray(_vao);
            _positionBuffer.bind();
            _positionBuffer.setData(*points);
            _colorBuffer.bind();
            _colorBuffer.setData(_colors);
            _highlightBuffer.bind();
            _highlightBuffer.setData(_highlights);
            _scalarBuffer.bind();
            _scalarBuffer.setData(_scalarProperty);
            glBindVertexArray(0);
        }

        void PointRenderer::setColors(const std::vector<Vector3f>& colors)
        {
            _colors = colors;

            glBindVertexArray(_vao);
            _colorBuffer.bind();
            _colorBuffer.setData(colors);
            glBindVertexArray(0);
        }

        void PointRenderer::setHighlight(const std::vector<char>& highlights)
        {
            _highlights = highlights;

            glBindVertexArray(_vao);
            _highlightBuffer.bind();
            _highlightBuffer.setData(highlights);
            glBindVertexArray(0);
        }

        void PointRenderer::setScalarProperty(const std::vector<float>& scalarProperty)
        {
            _scalarProperty = scalarProperty;

            glBindVertexArray(_vao);
            _scalarBuffer.bind();
            _scalarBuffer.setData(scalarProperty);
            glBindVertexArray(0);
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

            glGenVertexArrays(1, &_vao);
            glBindVertexArray(_vao);

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

            _colorBuffer.create();
            _colorBuffer.bind();
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
            glVertexAttribDivisor(2, 1);
            glEnableVertexAttribArray(2);

            _highlightBuffer.create();
            _highlightBuffer.bind();
            glVertexAttribIPointer(3, 1, GL_UNSIGNED_BYTE, 0, 0);
            glVertexAttribDivisor(3, 1);
            glEnableVertexAttribArray(3);

            _scalarBuffer.create();
            _scalarBuffer.bind();
            glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 0, 0);
            glVertexAttribDivisor(4, 1);
            glEnableVertexAttribArray(4);

            if (_numPoints > 0)
            {
                glBindVertexArray(_vao);
                _positionBuffer.bind();
                _positionBuffer.setData(*_positions);
                _colorBuffer.bind();
                _colorBuffer.setData(_colors);
                _highlightBuffer.bind();
                _highlightBuffer.setData(_highlights);
                _scalarBuffer.bind();
                _scalarBuffer.setData(_scalarProperty);
            }

            bool loaded = true;
            loaded &= _shader.loadShaderFromFile(":shaders/PointPlot.vert", ":shaders/PointPlot.frag");
            loaded &= _selectionShader.loadShaderFromFile(":shaders/SelectionBox.vert", ":shaders/Color.frag");
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
            
            float minBound = _bounds.width() < _bounds.height() ? _bounds.width() : _bounds.height();
            switch (_pointSettings._scalingMode) {
            case Relative: _shader.uniform1f("pointSize", _pointSettings._pointSize * minBound / 800); break;
            case Absolute: _shader.uniform1f("pointSize", _pointSettings._pointSize * minBound / size); break;
            }

            _shader.uniform1f("alpha", _pointSettings._alpha);
            _shader.uniformMatrix3f("projMatrix", _ortho);
            _shader.uniform1i("scalarEffect", _pointEffect);

            glBindVertexArray(_vao);
            glDrawArraysInstanced(GL_TRIANGLES, 0, 6, _numPoints);
            glBindVertexArray(0);
        }

        void PointRenderer::destroy()
        {
            glDeleteVertexArrays(1, &_vao);
            _positionBuffer.destroy();
            _colorBuffer.destroy();
        }

        void PointRenderer::onSelecting(Selection selection)
        {
            _selection = selection;

            _isSelecting = true;
        }

        void PointRenderer::onSelection(Selection selection)
        {
            _isSelecting = false;
        }
    } // namespace gui

} // namespace hdps
