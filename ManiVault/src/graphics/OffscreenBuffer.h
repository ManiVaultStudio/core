// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QWindow>
#include <QOpenGLContext>

class CORE_EXPORT OffscreenBuffer : public QWindow
{
public:
    OffscreenBuffer()
    {
        setSurfaceType(QWindow::OpenGLSurface);

        _context = new QOpenGLContext(this);
        _context->setFormat(requestedFormat());

        if (!_context->create())
            qFatal("Cannot create requested OpenGL context.");

        create();
    }

    QOpenGLContext* getContext() { return _context; }

    void bindContext()
    {
        _context->makeCurrent(this);
    }

    void releaseContext()
    {
        _context->doneCurrent();
    }

private:
    QOpenGLContext* _context;
};
