#pragma once

#include "Renderer.h"

#include "../graphics/Selection.h"
#include "../graphics/Matrix3f.h"

namespace hdps
{
    namespace gui
    {
        class SelectionRenderer : protected Renderer
        {
        public:
            virtual void init();
            virtual void resize(QSize renderSize);
            virtual void render();
            virtual void destroy();

            void onSelection(Selection selection);

        private:
            const Matrix3f toClipCoordinates = Matrix3f(2, 0, 0, 2, -1, -1);
            Matrix3f toNormalisedCoordinates;

            QSize _renderSize;

            ShaderProgram _shader;

            Selection _selection;
        };
    }
}
