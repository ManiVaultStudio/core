#pragma once

#include "Renderer.h"
#include "SelectionListener.h"

#include "../graphics/Matrix3f.h"

namespace hdps
{
    namespace gui
    {
        class SelectionRenderer : protected Renderer, public plugin::SelectionListener
        {
        public:
            virtual void init();
            virtual void resize(QSize renderSize);
            virtual void render();
            virtual void destroy();

            virtual void onSelecting(Selection selection);
            virtual void onSelection(Selection selection);

        private:
            const Matrix3f toClipCoordinates = Matrix3f(2, 0, 0, 2, -1, -1);

            QSize _renderSize;

            ShaderProgram _shader;

            Selection _selection;
        };
    }
}
