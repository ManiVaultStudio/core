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

#include "actions/WidgetAction.h"

#include <QOpenGLFunctions_3_3_Core>
#include <QSize>

#include "graphics/Shader.h"

namespace mv
{

class CORE_EXPORT Renderer : public QObject, protected QOpenGLFunctions_3_3_Core
{
protected:

    /**
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    explicit Renderer(QObject* parent = nullptr) :
        QObject(parent)
    {
    }

    virtual void init() = 0;
    virtual void resize(QSize renderSize) = 0;
    
    /**
     * Get the render size
     * @return Render size
     */
    virtual QSize getRenderSize() const = 0;

    /** Begin rendering */
    virtual void beginRender() = 0;

    /** Render */
    virtual void render() = 0;

    /** End rendering */
    virtual void endRender() = 0;

    /** Destroy the renderer */
    virtual void destroy() = 0;
};

}
