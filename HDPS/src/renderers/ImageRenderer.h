#pragma once

#include "Renderer.h"

namespace hdps
{
    class ImageRenderer : public Renderer
    {
    public:
        void init() override;
        void resize(QSize renderSize) override;
        void render() override;
        void destroy() override;
    private:
        ShaderProgram _shader;
    };
} // namespace hdps
