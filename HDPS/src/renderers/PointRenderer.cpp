#include "PointRenderer.h"

#include <limits>

#include <QDebug>

namespace hdps
{
    namespace gui
    {
        namespace
        {
            /**
             * Builds an orthographic projection matrix that transforms the given bounds
             * to the range [-1, 1] in both directions.
             */
            Matrix3f createProjectionMatrix(Bounds bounds)
            {
                Matrix3f m;
                m.setIdentity();
                m[0] = 2 / bounds.getWidth();
                m[4] = 2 / bounds.getHeight();
                m[6] = -((bounds.getRight() + bounds.getLeft()) / bounds.getWidth());
                m[7] = -((bounds.getTop() + bounds.getBottom()) / bounds.getHeight());
                return m;
            }
        }

        void PointArrayObject::init()
        {
            initializeOpenGLFunctions();

            // Generate a VAO for all instanced points
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

            // Vertex buffer
            BufferObject quad;
            quad.create();
            quad.bind();
            quad.setData(vertices);
            glVertexAttribPointer(ATTRIBUTE_VERTICES, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
            glEnableVertexAttribArray(ATTRIBUTE_VERTICES);

            // Position buffer
            _positionBuffer.create();
            _positionBuffer.bind();
            glVertexAttribPointer(ATTRIBUTE_POSITIONS, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
            glVertexAttribDivisor(ATTRIBUTE_POSITIONS, 1);
            glEnableVertexAttribArray(ATTRIBUTE_POSITIONS);

            // Highlight buffer, disabled by default
            _highlightBuffer.create();
            _highlightBuffer.bind();
            glVertexAttribIPointer(ATTRIBUTE_HIGHLIGHTS, 1, GL_BYTE, 0, nullptr);
            glVertexAttribDivisor(ATTRIBUTE_HIGHLIGHTS, 1);

            // Scalar buffer, disabled by default
            _scalarBuffer.create();
            _scalarBuffer.bind();
            glVertexAttribPointer(ATTRIBUTE_SCALARS, 1, GL_FLOAT, GL_FALSE, 0, nullptr);
            glVertexAttribDivisor(ATTRIBUTE_SCALARS, 1);

            // Color buffer, disabled by default
            _colorBuffer.create();
            _colorBuffer.bind();
            glVertexAttribPointer(ATTRIBUTE_COLORS, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
            glVertexAttribDivisor(ATTRIBUTE_COLORS, 1);
        }

        void PointArrayObject::setPositions(const std::vector<Vector2f>& positions)
        {
            if (positions.empty())
                return;

            _positions = positions;

            _dirtyPositions = true;
            _hasPositions = true;
        }

        void PointArrayObject::setHighlights(const std::vector<char>& highlights)
        {
            if (highlights.empty())
                return;

            _highlights = highlights;

            _dirtyHighlights = true;
            _hasHighlights = true;
        }

        void PointArrayObject::setScalars(const std::vector<float>& scalars)
        {
            if (scalars.empty())
                return;

            _scalarLow = std::numeric_limits<float>::max();
            _scalarHigh = -std::numeric_limits<float>::max();

            // Determine scalar range
            for (const float& scalar : scalars)
            {
                if (scalar < _scalarLow) _scalarLow = scalar;
                if (scalar > _scalarHigh) _scalarHigh = scalar;
            }
            _scalarRange = _scalarHigh - _scalarLow;

            _scalars = scalars;

            _dirtyScalars = true;
            _hasScalars = true;
        }

        void PointArrayObject::setColors(const std::vector<Vector3f>& colors)
        {
            if (colors.empty())
                return;

            _colors = colors;

            _dirtyColors = true;
            _hasColors = true;
        }

        void PointArrayObject::enableAttribute(uint index, bool enable)
        {
            glBindVertexArray(_handle);
            if (enable)
                glEnableVertexAttribArray(index);
            else
                glDisableVertexAttribArray(index);
        }

        void PointArrayObject::draw()
        {
            glBindVertexArray(_handle);

            if (_dirtyPositions)
            {
                _positionBuffer.bind();
                _positionBuffer.setData(_positions);
                _dirtyPositions = false;
            }
            if (_dirtyHighlights)
            {
                _highlightBuffer.bind();
                _highlightBuffer.setData(_highlights);
                enableAttribute(ATTRIBUTE_HIGHLIGHTS, true);
                _dirtyHighlights = false;
            }
            if (_dirtyScalars)
            {
                _scalarBuffer.bind();
                _scalarBuffer.setData(_scalars);
                enableAttribute(ATTRIBUTE_SCALARS, true);
                _dirtyScalars = false;
            }
            if (_dirtyColors)
            {
                _colorBuffer.bind();
                _colorBuffer.setData(_colors);
                enableAttribute(ATTRIBUTE_COLORS, true);
                _dirtyColors = false;
            }

            glDrawArraysInstanced(GL_TRIANGLES, 0, 6, _positions.size());
            glBindVertexArray(0);
        }

        void PointArrayObject::destroy()
        {
            glDeleteVertexArrays(1, &_handle);
            _positionBuffer.destroy();
        }

        void PointRenderer::setData(const std::vector<Vector2f>& positions)
        {
            _gpuPoints.setPositions(positions);
        }

        void PointRenderer::setHighlights(const std::vector<char>& highlights)
        {
            _gpuPoints.setHighlights(highlights);
        }

        void PointRenderer::setScalars(const std::vector<float>& scalars)
        {
            _gpuPoints.setScalars(scalars);
        }

        void PointRenderer::setColors(const std::vector<Vector3f>& colors)
        {
            _gpuPoints.setColors(colors);
        }

        void PointRenderer::setScalarEffect(const PointEffect effect)
        {
            _pointEffect = effect;
        }

        void PointRenderer::setColormap(const QString colormap)
        {
            _colormap.loadFromFile(colormap);
        }

        void PointRenderer::setBounds(const Bounds& bounds)
        {
            _bounds = bounds;
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

        void PointRenderer::setOutlineColor(Vector3f color)
        {
            _outlineColor = color;
        }

        void PointRenderer::init()
        {
            initializeOpenGLFunctions();

            _gpuPoints.init();

            bool loaded = true;
            loaded &= _shader.loadShaderFromFile(":shaders/PointPlot.vert", ":shaders/PointPlot.frag");

            if (!loaded) {
                qCritical() << "Failed to load one of the Scatterplot shaders";
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
            _orthoM = createProjectionMatrix(_bounds);

            _shader.bind();

            switch (_pointSettings._scalingMode) {
            case Relative: _shader.uniform1f("pointSize", _pointSettings._pointSize); break;
            case Absolute: _shader.uniform1f("pointSize", _pointSettings._pointSize / size); break;
            }

            _shader.uniformMatrix3f("orthoM", _orthoM);
            _shader.uniform1f("alpha", _pointSettings._alpha);
            _shader.uniform1i("scalarEffect", _pointEffect);
            _shader.uniform3f("outlineColor", _outlineColor);

            _shader.uniform1i("hasHighlights", _gpuPoints.hasHighlights());
            _shader.uniform1i("hasScalars", _gpuPoints.hasScalars());
            _shader.uniform1i("hasColors", _gpuPoints.hasColors());

            if (_gpuPoints.hasScalars())
            {
                _shader.uniform3f("scalarRange", _gpuPoints.getScalarRange());
            }

            if (_pointEffect == PointEffect::Color) {
                _colormap.bind(0);
                _shader.uniform1i("colormap", 0);
            }

            _gpuPoints.draw();
        }

        void PointRenderer::destroy()
        {
            _gpuPoints.destroy();
        }

    } // namespace gui

} // namespace hdps
