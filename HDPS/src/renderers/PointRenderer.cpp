#include "PointRenderer.h"

#include <QDebug>
#include <iostream>

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

        void PointRenderer::setImageData(const std::vector<float>* data, unsigned int width, unsigned int height)
        {
            glBindTexture(GL_TEXTURE_2D, _texAtlas);

            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width * 250, height * 250, 0, GL_RED, GL_FLOAT, nullptr);

            const float* pixels = (data->data() + 1);
            for (int i = 0; i < 60000; i++)
            {
                int x = i % 250;
                int y = i / 250;

                glTexSubImage2D(GL_TEXTURE_2D, 0, x * width, y * height, width, height, GL_RED, GL_FLOAT, pixels);

                pixels += (width * height) + 1;
            }

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            GLenum error = glGetError();
            qDebug() << "ERROR: " << error;
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
            _bounds.setCoords(left, top, right, bottom);
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

        void PointRenderer::setPointMode(PointMode pointMode)
        {
            _pointMode = pointMode;
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
            loaded &= _texShader.loadShaderFromFile(":shaders/PointPlot.vert", ":shaders/ImagePlot.frag");
            loaded &= _quadShader.loadShaderFromFile(":shaders/Quad.vert", ":shaders/Texture.frag");

            if (!loaded) {
                qDebug() << "Failed to load one of the Scatterplot shaders";
            }

            glGenTextures(1, &_texAtlas);
        }

        void PointRenderer::resize(QSize renderSize)
        {
            int w = renderSize.width();
            int h = renderSize.height();

            _windowSize.setWidth(w);
            _windowSize.setHeight(h);
        }
        int iter = 1000;
        void PointRenderer::render()
        {
            int w = _windowSize.width();
            int h = _windowSize.height();
            int size = w < h ? w : h;
            glViewport(w / 2 - size / 2, h / 2 - size / 2, size, size);

            // World to clip transformation
            _ortho = createProjectionMatrix(_bounds);

            if (_pointMode == PointMode::Dot)
            {
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
            else if (_pointMode == PointMode::Image)
            {
                _texShader.bind();

                switch (_pointSettings._scalingMode) {
                case Relative: _texShader.uniform1f("pointSize", _pointSettings._pointSize); break;
                case Absolute: _texShader.uniform1f("pointSize", _pointSettings._pointSize / size); break;
                }

                _texShader.uniformMatrix3f("projMatrix", _ortho);
                _texShader.uniform1f("alpha", _pointSettings._alpha);
                //_texShader.uniform1i("scalarEffect", _pointEffect);

                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, _texAtlas);
                _texShader.uniform1i("texAtlas", 1);

                if (_pointEffect == PointEffect::Color) {
                    _colormap.bind(0);
                    _texShader.uniform1i("colormap", 0);
                }
                qDebug() << "DRAWING POINTS";
                glBindVertexArray(_gpuPoints._handle);
                glDrawArraysInstanced(GL_TRIANGLES, 0, 6, _numPoints);
                glBindVertexArray(0);

                //std::vector<float> pixelData(800 * 800 * 4);
                //glReadPixels(330, 20, 800, 800, GL_RGBA, GL_FLOAT, pixelData.data());

                //QImage pluto(800, 800, QImage::Format::Format_ARGB32);

                //for (int j = 0; j < 800; ++j)
                //{
                //  for (int i = 0; i < 800; ++i)
                //  {
                //    int idx = j * 800 + i;
                //    float r = (pixelData[idx * 4]);
                //    float g = (pixelData[idx * 4 + 1]);
                //    float b = (pixelData[idx * 4 + 2]);
                //    pluto.setPixel(i, 800-1-j, qRgb(r * 255, g * 255, b * 255));
                //  }
                //}
                
                //pluto.save(QString("embImage_") + QString::number(int(iter)) + QString(".png"));
                iter++;
            }
        }

        void PointRenderer::destroy()
        {
            _gpuPoints.destroy();
        }

    } // namespace gui

} // namespace hdps
