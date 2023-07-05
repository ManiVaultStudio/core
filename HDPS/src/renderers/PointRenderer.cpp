// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

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

            // Color buffer, disabled by default
            _colorBuffer.create();
            _colorBuffer.bind();

            glVertexAttribPointer(ATTRIBUTE_COLORS, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
            glVertexAttribDivisor(ATTRIBUTE_COLORS, 1);

            // Scalar buffer for point color, disabled by default
            _colorScalarBuffer.create();
            _colorScalarBuffer.bind();

            glVertexAttribPointer(ATTRIBUTE_SCALARS_COLOR, 1, GL_FLOAT, GL_FALSE, 0, nullptr);
            glVertexAttribDivisor(ATTRIBUTE_SCALARS_COLOR, 1);

            // Scalar buffer for point size, disabled by default
            _sizeScalarBuffer.create();
            _sizeScalarBuffer.bind();

            glVertexAttribPointer(ATTRIBUTE_SCALARS_SIZE, 1, GL_FLOAT, GL_FALSE, 0, nullptr);
            glVertexAttribDivisor(ATTRIBUTE_SCALARS_SIZE, 1);

            // Scalar buffer for point opacity, disabled by default
            _opacityScalarBuffer.create();
            _opacityScalarBuffer.bind();

            glVertexAttribPointer(ATTRIBUTE_SCALARS_OPACITY, 1, GL_FLOAT, GL_FALSE, 0, nullptr);
            glVertexAttribDivisor(ATTRIBUTE_SCALARS_OPACITY, 1);
        }

        void PointArrayObject::setPositions(const std::vector<Vector2f>& positions)
        {
            _positions = positions;

            _dirtyPositions = true;
        }

        void PointArrayObject::setHighlights(const std::vector<char>& highlights)
        {
            _highlights = highlights;

            _dirtyHighlights = true;
        }

        void PointArrayObject::setScalars(const std::vector<float>& scalars, bool adjustColorMapRange)
        {
            if (adjustColorMapRange)
            {
                _colorScalarsRange.x = std::numeric_limits<float>::max();
                _colorScalarsRange.y = -std::numeric_limits<float>::max();

                // Determine scalar range
                for (const float& scalar : scalars)
                {
                    if (scalar < _colorScalarsRange.x)
                        _colorScalarsRange.x = scalar;

                    if (scalar > _colorScalarsRange.y)
                        _colorScalarsRange.y = scalar;
                }

                _colorScalarsRange.z = _colorScalarsRange.y - _colorScalarsRange.x;

                if (_colorScalarsRange.z < 1e-07)
                    _colorScalarsRange.z = static_cast<float>(1e-07);
            }

            _colorScalars = scalars;

            _dirtyColorScalars = true;
        }

        void PointArrayObject::setSizeScalars(const std::vector<float>& scalars)
        {
            _sizeScalars = scalars;

            _dirtySizeScalars = true;
        }

        void PointArrayObject::setOpacityScalars(const std::vector<float>& scalars)
        {
            _opacityScalars = scalars;

            _dirtyOpacityScalars = true;
        }

        void PointArrayObject::setColors(const std::vector<Vector3f>& colors)
        {
            _colors = colors;

            _dirtyColors = true;
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

            if (_dirtyColors)
            {
                _colorBuffer.bind();
                _colorBuffer.setData(_colors);
                enableAttribute(ATTRIBUTE_COLORS, true);

                _dirtyColors = false;
            }

            if (_dirtyColorScalars)
            {
                _colorScalarBuffer.bind();
                _colorScalarBuffer.setData(_colorScalars);

                enableAttribute(ATTRIBUTE_SCALARS_COLOR, true);

                _dirtyColorScalars = false;
            }

            
            if (_dirtySizeScalars)
            {
                _sizeScalarBuffer.bind();
                _sizeScalarBuffer.setData(_sizeScalars);

                enableAttribute(ATTRIBUTE_SCALARS_SIZE, true);

                _dirtySizeScalars = false;
            }

            if (_dirtyOpacityScalars)
            {
                _opacityScalarBuffer.bind();
                _opacityScalarBuffer.setData(_opacityScalars);

                enableAttribute(ATTRIBUTE_SCALARS_OPACITY, true);

                _dirtyOpacityScalars = false;
            }

            // Before calling glDrawArraysInstanced, check if _positions is non-empty, to
            // prevent a crash on some (older) computers, see HDPS core pull request #42,
            // "Fix issue #34: Crash when opening scatterplot plugin", March 2020.
            if (!_positions.empty())
            {
                glDrawArraysInstanced(GL_TRIANGLES, 0, 6, (GLsizei) _positions.size());
            }
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

        void PointRenderer::setHighlights(const std::vector<char>& highlights, const std::int32_t& numSelectedPoints)
        {
            _gpuPoints.setHighlights(highlights);

            _numSelectedPoints = numSelectedPoints;
        }

        void PointRenderer::setColorChannelScalars(const std::vector<float>& scalars, bool adjustColorMapRange)
        {
            _gpuPoints.setScalars(scalars, adjustColorMapRange);
        }

        void PointRenderer::setSizeChannelScalars(const std::vector<float>& scalars)
        {
            _gpuPoints.setSizeScalars(scalars);
        }

        void PointRenderer::setOpacityChannelScalars(const std::vector<float>& scalars)
        {
            _gpuPoints.setOpacityScalars(scalars);
        }

        void PointRenderer::setColors(const std::vector<Vector3f>& colors)
        {
            _gpuPoints.setColors(colors);
        }

        void PointRenderer::setScalarEffect(const PointEffect effect)
        {
            _pointEffect = effect;
        }

        void PointRenderer::setColormap(const QImage& image)
        {
            _colormap.loadFromImage(image);
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

        hdps::gui::PointSelectionDisplayMode PointRenderer::getSelectionDisplayMode() const
        {
            return _selectionDisplayMode;
        }

        void PointRenderer::setSelectionDisplayMode(PointSelectionDisplayMode selectionDisplayMode)
        {
            _selectionDisplayMode = selectionDisplayMode;
        }

        hdps::Vector3f PointRenderer::getSelectionOutlineColor() const
        {
            return _selectionOutlineColor;
        }

        void PointRenderer::setSelectionOutlineColor(Vector3f color)
        {
            _selectionOutlineColor = color;
        }

        bool PointRenderer::getSelectionOutlineOverrideColor() const
        {
            return _selectionOutlineOverrideColor;
        }

        void PointRenderer::setSelectionOutlineOverrideColor(float selectionOutlineOverrideColor)
        {
            _selectionOutlineOverrideColor = selectionOutlineOverrideColor;
        }

        float PointRenderer::getSelectionOutlineScale() const
        {
            return _selectionOutlineScale;
        }

        void PointRenderer::setSelectionOutlineScale(float selectionOutlineScale)
        {
            _selectionOutlineScale = selectionOutlineScale;
        }

        float PointRenderer::getSelectionOutlineOpacity() const
        {
            return _selectionOutlineOpacity;
        }

        void PointRenderer::setSelectionOutlineOpacity(float selectionOutlineOpacity)
        {
            _selectionOutlineOpacity = selectionOutlineOpacity;
        }

        bool PointRenderer::getSelectionHaloEnabled() const
        {
            return _selectionHaloEnabled;
        }

        void PointRenderer::setSelectionHaloEnabled(float selectionHaloEnabled)
        {
            _selectionHaloEnabled = selectionHaloEnabled;
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

            // Point size uniforms
            bool absoluteRendering = _pointSettings._scalingMode == PointScaling::Absolute;
            _shader.uniform1f("pointSize", _pointSettings._pointSize);
            _shader.uniform1f("pointSizeScale", absoluteRendering ? (1.0 / size) : 1.0f / size);

            _shader.uniformMatrix3f("orthoM", _orthoM);
            _shader.uniform1f("pointOpacity", _pointSettings._alpha);
            _shader.uniform1i("scalarEffect", _pointEffect);
            
            _shader.uniform4f("dataBounds", _bounds.getLeft(), _bounds.getRight(), _bounds.getBottom(), _bounds.getTop());

            _shader.uniform1i("selectionDisplayMode", static_cast<std::int32_t>(_selectionDisplayMode));
            _shader.uniform1f("selectionOutlineScale", _selectionOutlineScale);
            _shader.uniform3f("selectionOutlineColor", _selectionOutlineColor);
            _shader.uniform1i("selectionOutlineOverrideColor", _selectionOutlineOverrideColor);
            _shader.uniform1f("selectionOutlineOpacity", _selectionOutlineOpacity);
            _shader.uniform1i("selectionHaloEnabled", _selectionHaloEnabled);

            _shader.uniform1i("hasHighlights", _gpuPoints.hasHighlights());
            _shader.uniform1i("hasScalars", _gpuPoints.hasColorScalars());
            _shader.uniform1i("hasColors", _gpuPoints.hasColors());
            _shader.uniform1i("hasSizes", _gpuPoints.hasSizeScalars());
            _shader.uniform1i("hasOpacities", _gpuPoints.hasOpacityScalars());
            _shader.uniform1i("numSelectedPoints", _numSelectedPoints);

            if (_gpuPoints.hasColorScalars())
                _shader.uniform3f("colorMapRange", _gpuPoints.getColorMapRange());

            if (_colormap.isCreated() && (_pointEffect == PointEffect::Color || _pointEffect == PointEffect::Color2D))
            {
                _colormap.bind(0);
                _shader.uniform1i("colormap", 0);
            }

            _gpuPoints.draw();
        }

        void PointRenderer::destroy()
        {
            _gpuPoints.destroy();
        }

        hdps::Vector3f PointRenderer::getColorMapRange() const
        {
            return _gpuPoints.getColorMapRange();
        }

        void PointRenderer::setColorMapRange(const float& min, const float& max)
        {
            return _gpuPoints.setColorMapRange(Vector3f(min, max, max - min));
        }

    } // namespace gui

} // namespace hdps
