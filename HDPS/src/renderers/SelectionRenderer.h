#pragma once

#include "Renderer.h"
#include "../SelectionListener.h"

#include "../graphics/Matrix3f.h"

namespace hdps
{
    namespace gui
    {
        class SelectionRenderer : protected Renderer, public plugin::SelectionListener
        {
        public:
            void init() override;
            void resize(QSize renderSize) override;
            void render() override;
            void destroy() override;

            void onSelecting(Selection selection) override;
            void onSelection(Selection selection) override;

        private:
            const Matrix3f toClipCoordinates = Matrix3f(2, 0, 0, 2, -1, -1);

            QSize _renderSize;

            ShaderProgram _shader;

            Selection _selection;

            bool _selecting = false;
        };
    }
}
