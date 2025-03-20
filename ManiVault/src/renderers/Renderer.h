// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

/**
 * Renderer.h
 *
 * Provides the interface for a Renderer which can be used in a QOpenGLWidget.
 */

#pragma once

#include "ManiVaultGlobals.h"

#include <QOpenGLFunctions_3_3_Core>
#include <QSize>

#include "graphics/Shader.h"

namespace mv
{
    class CORE_EXPORT Renderer : public QObject, protected QOpenGLFunctions_3_3_Core
    {
    public:
        explicit Renderer(QObject* parent = nullptr) : QObject(parent) {}

    protected:
        virtual void init() = 0;
        virtual void resize(QSize renderSize) = 0;
        virtual void render() = 0;
        virtual void destroy() = 0;
    };
}
