#pragma once

#include "Renderer.h"

namespace hdps
{
    class ImageRenderer : public Renderer
    {
    public:
        virtual void init() override;
        virtual void resize(QSize renderSize) override;
        virtual void render() override;
        virtual void destroy() override;
    private:
        ShaderProgram _shader;
    };
} // namespace hdps
