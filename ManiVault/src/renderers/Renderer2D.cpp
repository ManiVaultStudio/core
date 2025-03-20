// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Renderer2D.h"

#ifdef _DEBUG
    //#define RENDERER_2D_VERBOSE
#endif

#define RENDERER_2D_VERBOSE

namespace mv
{

Renderer2D::Renderer2D(QObject* parent) :
    Renderer(parent),
    _navigator(*this)
{
}

void Renderer2D::resize(QSize renderSize)
{
    _renderSize = renderSize;
}

QSize Renderer2D::getRenderSize() const
{
    return _renderSize;
}

Navigator2D& Renderer2D::getNavigator()
{
	return _navigator;
}

void Renderer2D::beginRender()
{
#ifdef RENDERER_2D_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    const auto size = _renderSize.width() < _renderSize.height() ? _renderSize.width() : _renderSize.height();

    glViewport(_renderSize.width() / 2 - size / 2, _renderSize.height() / 2 - size / 2, size, size);
}

void Renderer2D::endRender()
{
#ifdef RENDERER_2D_VERBOSE
    qDebug() << __FUNCTION__;
#endif
}

}
