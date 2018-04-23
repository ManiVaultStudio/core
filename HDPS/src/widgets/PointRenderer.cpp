#include "PointRenderer.h"

#include <QDebug>

namespace hdps
{
    namespace gui
    {
        // Positions need to be passed as a pointer as we need to store them locally in order
        // to be able to find the subset of data that's part of a selection. If passed
        // by reference then we can upload the data to the GPU, but not store it in the widget.
        void PointRenderer::setData(const std::vector<Vector2f>* points)
        {
            qDebug() << "Setting data";
            _numPoints = (unsigned int) points->size();
            _positions = points;
            _colors.clear();
            _colors.resize(_numPoints, Vector3f(0.5f, 0.5f, 0.5f));

            glBindVertexArray(_vao);
            _positionBuffer.bind();
            _positionBuffer.setData(*points);
            _colorBuffer.bind();
            _colorBuffer.setData(_colors);
            glBindVertexArray(0);
            qDebug() << "Done setting data";
        }

        void PointRenderer::setColors(const std::vector<Vector3f>& colors)
        {
            qDebug() << "Setting colours";
            _colors = colors;

            glBindVertexArray(_vao);
            _colorBuffer.bind();
            _colorBuffer.setData(colors);
            glBindVertexArray(0);
            qDebug() << "Done setting colours";
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
            _pointSize = size;
        }

        void PointRenderer::setSelectionColor(const Vector3f selectionColor)
        {
            _selectionColor = selectionColor;
        }

        void PointRenderer::setAlpha(const float alpha)
        {
            _alpha = alpha;
            _alpha = _alpha > 1 ? 1 : _alpha;
            _alpha = _alpha < 0 ? 0 : _alpha;
        }

        void PointRenderer::setPointScaling(PointScaling scalingMode)
        {
            _scalingMode = scalingMode;
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

            if (_numPoints > 0)
            {
                glBindVertexArray(_vao);
                _positionBuffer.bind();
                _positionBuffer.setData(*_positions);
                _colorBuffer.bind();
                _colorBuffer.setData(_colors);
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

            int size = w < h ? w : h;

            float wAspect = (float)w / size;
            float hAspect = (float)h / size;
            float wDiff = ((wAspect - 1) / 2.0);
            float hDiff = ((hAspect - 1) / 2.0);

            toIsotropicCoordinates = Matrix3f(wAspect, 0, 0, hAspect, -wDiff, -hDiff);
        }

        void PointRenderer::render()
        {
            qDebug() << "Rendering points";

            int w = _windowSize.width();
            int h = _windowSize.height();
            int size = w < h ? w : h;
            glViewport(w / 2 - size / 2, h / 2 - size / 2, size, size);

            // World to clip transformation
            _ortho.setIdentity();
            _ortho[0] = 2 / (_bounds.right() - _bounds.left());
            _ortho[4] = 2 / (_bounds.top() - _bounds.bottom());
            _ortho[6] = -((_bounds.right() + _bounds.left()) / (_bounds.right() - _bounds.left()));
            _ortho[7] = -((_bounds.top() + _bounds.bottom()) / (_bounds.top() - _bounds.bottom()));

            _shader.bind();
            switch (_scalingMode) {
            case Relative: _shader.uniform1f("pointSize", _pointSize / 800); break;
            case Absolute: _shader.uniform1f("pointSize", _pointSize / _windowSize.width()); break;
            }

            _shader.uniform3f("selectionColor", _selectionColor.x, _selectionColor.y, _selectionColor.z);
            _shader.uniform1f("alpha", _alpha);
            _shader.uniform1f("maxDimension", _maxDimension);
            //_shader.uniform1i("selecting", _selecting);
            _shader.uniform2f("start", topLeft.x, topLeft.y);
            _shader.uniform2f("end", bottomRight.x, bottomRight.y);

            glBindVertexArray(_vao);
            glDrawArraysInstanced(GL_TRIANGLES, 0, 6, _numPoints);
            glBindVertexArray(0);

            //if (_selecting)
            //{
            //    topLeft = toClipCoordinates * _selection.topLeft();
            //    bottomRight = toClipCoordinates * _selection.bottomRight();

            //    glViewport(0, 0, w, h);

            //    // Selection
            //    _selectionShader.bind();
            //    _selectionShader.uniform2f("start", topLeft.x, topLeft.y);
            //    _selectionShader.uniform2f("end", bottomRight.x, bottomRight.y);
            //    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            //}
            qDebug() << "Done rendering points";
        }

        void PointRenderer::destroy()
        {
            glDeleteVertexArrays(1, &_vao);
            _positionBuffer.destroy();
            _colorBuffer.destroy();
        }

        void PointRenderer::onSelection(Selection selection)
        {
            std::vector<unsigned int> indices;
            for (unsigned int i = 0; i < _numPoints; i++)
            {
                Vector2f point = (*_positions)[i];
                point.x *= _windowSize.width() / _windowSize.height();

                if (selection.contains(point))
                    indices.push_back(i);
            }
        }

    } // namespace gui

} // namespace hdps
