// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "Renderer.h"
#include "Navigator2D.h"

namespace mv
{

/**
 * Renderer 2D class
 *
 * Supports two-dimensional rendering:
 * - Orchestrates panning and zooming using Navigator2D
 * - Sets up the matrix transformations
 * - Renders 2D data
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT Renderer2D : public Renderer
{

Q_OBJECT

public:

    /**
     * Construct a new two-dimensional renderer
     *
     * @param parent Pointer to the parent object
     */
    explicit Renderer2D(QObject* parent = nullptr);

	/**
     * Resize the renderer to \p renderSize
     * @param renderSize New size of the renderer
	 */
	void resize(QSize renderSize) override;

    /**
     * Get the render size
     * @return Render size
     */
    QSize getRenderSize() const override;

    /**
     * Get the 2D navigator
     *
     * @return Reference to the 2D navigator
     */
    Navigator2D& getNavigator();

protected:

    /** Begin rendering */
    void beginRender() override;

    /** End rendering */
    void endRender() override;

private:
    QSize           _renderSize;    /** Size of the renderer canvas */
    Navigator2D     _navigator;     /** 2D navigator */

    friend class Navigator2D;
};

}
