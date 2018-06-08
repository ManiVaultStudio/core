/**
 * Renderer.h
 *
 * Provides the interface for a Renderer which can be used in a QOpenGLWidget.
 */

#pragma once

#include <QOpenGLFunctions_3_3_Core>
#include <QSize>

#include "../graphics/Shader.h"

namespace hdps
{
    class Renderer : protected QOpenGLFunctions_3_3_Core
    {
        virtual void init() = 0;
        virtual void resize(QSize renderSize) = 0;
        virtual void render() = 0;
        virtual void destroy() = 0;
    };
}
