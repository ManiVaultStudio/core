// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QObject>
#include <QVector2D>
#include <QMatrix4x4>

namespace mv
{

class Renderer2D;

/**
 * Navigator 2D class
 *
 * Orchestrates panning and zooming in a widget that displays 2D data using Renderer2D
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT Navigator2D : public QObject
{

Q_OBJECT

public:

    /**
     * Construct a new two-dimensional navigator
     *
     * @param renderer Reference to parent renderer
     * @param parent Pointer to the parent object
     */
    explicit Navigator2D(Renderer2D& renderer, QObject* parent = nullptr);

    /**
     * Initializes the two-dimensional navigator with a \p sourceWidget
     * @param sourceWidget Pointer to the renderer widget
     */
    void initialize(QWidget* sourceWidget);

    /**
     * Watch \p watched for events
     *
     * @param watched Watched object
     * @param event Event
     * @return True if the event was handled, false otherwise
     */
    bool eventFilter(QObject* watched, QEvent* event) override;

    /**
     * Get the view matrix
     * @return View matrix
     */
    QMatrix4x4 getViewMatrix() const;

    /**
     * Get the zoom rectangle
     * @return Zoom rectangle
     */
    QRectF getZoomRectangle() const;

public: // Navigation

    /**
     * Zoom by \p factor around \p center
     * @param center Point to zoom around
     * @param factor Zoom factor
     */
    void zoomAround(const QPoint& center, float factor);

    /**
     * Zoom to \p zoomRectangle
     * @param zoomRectangle Zoom to this rectangle
     */
    void zoomToRectangle(const QRectF& zoomRectangle);

    /**
     * Pan by \p delta
     * @param delta Pan by this amount
     */
    void panBy(const QPointF& delta);

    /** Zoom to extents of the data bounds (with a margin around it) */
    void resetView();

    /**
     * Get whether the renderer is panning
     * @return Boolean determining whether the renderer is panning
     */
    bool isPanning() const;

    /**
     * Get whether the renderer is zooming
     * @return Boolean determining whether the renderer is zooming
     */
    bool isZooming() const;

    /**
     * Get whether the renderer is navigating
     * @return Boolean determining whether the renderer is navigating
     */
    bool isNavigating() const;

protected: // Navigation

    /**
     * Set whether the renderer is panning to \p isPanning
     * @param isPanning Boolean determining whether the renderer is panning
     */
    void setIsPanning(bool isPanning);

    /**
     * Set whether the renderer is zooming to \p isZooming
     * @param isZooming Boolean determining whether the renderer is zooming
     */
    void setIsZooming(bool isZooming);

    /**
     * Set whether the renderer is navigating to \p isNavigating
     * @param isNavigating Boolean determining whether the renderer is navigating
     */
    void setIsNavigating(bool isNavigating);

    /** Panning has begun */
    void beginPanning();

    /** Panning has ended */
    void endPanning();

    /** Zooming has begun */
    void beginZooming();

    /** Zooming has ended */
    void endZooming();

    /** Navigation has begun */
    void beginNavigation();

    /** Navigation has ended */
    void endNavigation();

signals:

    /** Signals that panning has started */
    void panningStarted();

    /** Signals that panning has ended */
    void panningEnded();

    /**
     * Signals that is panning changed to \p isPanning
     * @param isPanning
     */
    void isPanningChanged(bool isPanning);

    /** Signals that zooming has started */
    void zoomingStarted();

    /** Signals that zooming has ended */
    void zoomingEnded();

    /**
     * Signals that is zooming changed to \p isZooming
     * @param isZooming Boolean determining whether the renderer is zooming
     */
    void isZoomingChanged(bool isZooming);

    /** Signals that navigation has started */
    void navigationStarted();

    /** Signals that navigation has ended */
    void navigationEnded();

    /**
     * Signals that is navigating changed to \p isNavigating
     * @param isNavigating Boolean determining whether the renderer is navigating
     */
    void isNavigatingChanged(bool isNavigating);

private:
    QPointer<QWidget>       _sourceWidget;              /** Source widget for panning and zooming */
    Renderer2D&             _renderer;                  /** Reference to parent renderer */
    bool                    _initialized;               /** Initialized flag */
    QVector<QPoint>         _mousePositions;            /** Recorded mouse positions */
    bool                    _isNavigating;              /** Navigating flag */
    bool                    _isPanning;                 /** Panning flag */
    bool                    _isZooming;                 /** Zooming flag */
    float                   _zoomFactor;                /** Zoom factor */

private:
    QPointF                 _zoomRectangleTopLeft;      /** Zoom rectangle top-left in world coordinates */
    QSizeF                  _zoomRectangleSize;         /** Zoom rectangle size in world coordinates */
    float                   _zoomRectangleMargin;       /** Zoom rectangle margin */
};

}
