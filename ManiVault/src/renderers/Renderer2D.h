// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "Renderer.h"

namespace mv
{

/**
 * Renderer 2D class
 *
 * Supports two-dimensional rendering:
 * - Organizes panning and zooming
 * - Sets up the matrix transformations
 * - Renders 2D data
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT Renderer2D : public Renderer
{
public:

    /**
     * Construct a new two-dimensional renderer
     *
     * @param sourceWidget If set, use this widget to do panning and zooming
     * @param parent Pointer to the parent object
     */
    explicit Renderer2D(QWidget* sourceWidget = nullptr, QObject* parent = nullptr);

private:
    QPointer<QWidget>   _sourceWidget;      /** Source widget for panning and zooming */
};

}
