#include "SelectionRenderer.h"

#include <QDebug>

namespace hdps
{
    namespace gui
    {
        void SelectionRenderer::init()
        {
            initializeOpenGLFunctions();

            bool loaded = _shader.loadShaderFromFile(":shaders/SelectionBox.vert", ":shaders/Color.frag");
            if (!loaded)
                qDebug() << "Failed to load selection shader";
        }

        void SelectionRenderer::resize(QSize renderSize)
        {
            _renderSize = renderSize;

            int w = renderSize.width();
            int h = renderSize.height();
        }

        void SelectionRenderer::render()
        {
            Vector2f topLeft = toClipCoordinates * _selection.topLeft();
            Vector2f bottomRight = toClipCoordinates * _selection.bottomRight();

            glViewport(0, 0, _renderSize.width(), _renderSize.height());

            _shader.bind();
            _shader.uniform2f("start", topLeft.x, topLeft.y);
            _shader.uniform2f("end", bottomRight.x, bottomRight.y);

            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }

        void SelectionRenderer::destroy()
        {
            _shader.destroy();
        }

        void SelectionRenderer::onSelecting(Selection selection)
        {
            _selection = selection;
        }

        void SelectionRenderer::onSelection(Selection selection)
        {
            _selection = selection;
        }

        Matrix3f SelectionRenderer::getTransformMatrix()
        {
            return Matrix3f();
        }
    }
}
