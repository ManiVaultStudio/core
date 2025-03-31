// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "Renderer.h"
#include "Navigator2D.h"

#include <QMatrix4x4>

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
     * @return Reference to the 2D navigator
     */
    Navigator2D& getNavigator();

    /**
     * Get the 2D navigator
     * @return Reference to the 2D navigator
     */
    const Navigator2D& getNavigator() const;

public: // Coordinate conversions

    /**
     * Convert \p screenPoint to point in world coordinates using \p modelViewMatrix
     * @param modelViewMatrix Model-view matrix
     * @param screenPoint Point in screen coordinates [0..width, 0..height]
     * @return Position in world coordinates
     */
    QVector3D getScreenPointToWorldPosition(const QMatrix4x4& modelViewMatrix, const QPoint& screenPoint) const;

    /**
     * Convert \p position in world coordinates to point in normalized screen coordinates
     * @param position Position in world coordinates
     * @return Point in normalized screen coordinates [-1..1, -1..1]
     */
    QVector2D getWorldPositionToNormalizedScreenPoint(const QVector3D& position) const;

    /**
     * Convert \p position in world coordinates to point in screen coordinates
     * @param position Position in world coordinates
     * @return Point in screen coordinates [0..width, 0..height]
     */
    QPoint getWorldPositionToScreenPoint(const QVector3D& position) const;

    /**
     * Convert \p screenPoint to point in normalized screen coordinates
     * @param screenPoint Point in screen coordinates [0..width, 0..height]
     * @return Point in normalized screen coordinates [-1..1, -1..1]
     */
    QVector2D getScreenPointToNormalizedScreenPoint(const QVector2D& screenPoint) const;

    /** Returns the matrix that converts screen coordinates [0..width, 0..height] to normalized screen coordinates [-1..1, -1..1] */
    QMatrix4x4 getScreenToNormalizedScreenMatrix() const;

    /** Returns the matrix that converts normalized screen coordinates [-1..1, -1..1] to screen coordinates [0..width, 0..height] */
    QMatrix4x4 getNormalizedScreenToScreenMatrix() const;

	float getZoomPercentage() const;

	/** Returns the projection matrix */
    QMatrix4x4 getProjectionMatrix() const;

    /**
     * Get screen bounding rectangle from world bounding rectangle
     * @param worldBoundingRectangle World bounding rectangle
     */
    QRect getScreenRectangleFromWorldRectangle(const QRectF& worldBoundingRectangle) const;

	/**
     * Get model matrix
     * @return Model matrix
	 */
	QMatrix4x4 getModelMatrix() const;

    /**
     * Set model matrix to \p modelMatrix
     * @param modelMatrix Model matrix
     */
    void setModelMatrix(const QMatrix4x4& modelMatrix);

	/**
	 * Get model-view-projection matrix
	 * @return Model-view-projection matrix
	 */
	QMatrix4x4 getModelViewProjectionMatrix() const;

protected:

    /** Begin rendering (sets up the OpenGL viewport and computes the model-view-projection matrix) */
    void beginRender() override;

    /** End rendering */
    void endRender() override;

public:

	/**
	 * Get data bounds
     * @return Data bounds
	 */
	QRectF getDataBounds() const;

    /**
     * Set data bounds to \p dataBounds
     * @param dataBounds Data bounds
     */
    virtual void setDataBounds(const QRectF& dataBounds);

private:

    /** Update the model-view-projection matrix */
    void updateModelViewProjectionMatrix();

private:
    QSize           _renderSize;                    /** Size of the renderer canvas */
    Navigator2D     _navigator;                     /** 2D navigator */
    QRectF          _dataBounds;                    /** Bounds of the data */
    QMatrix4x4      _modelMatrix;   
    QMatrix4x4      _modelViewProjectionMatrix;     /** Model-view-projection matrix */

    friend class Navigator2D;
};

}
