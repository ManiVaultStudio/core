// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <actions/NavigationAction.h>

#include <QObject>
#include <QTimer>
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

    /** For drawing the zoom region */
    class ZoomOverlayWidget : public gui::OverlayWidget
    {
    public:

        /**
         * Construct a new zoom overlay widget
         * @param navigator Reference to the navigator
         * @param targetWidget Pointer to the target widget
         */
        ZoomOverlayWidget(Navigator2D& navigator, QWidget* targetWidget);

        /**
         * Override the paint event to draw the zoom regio rectangle
         * @param event 
         */
        void paintEvent(QPaintEvent* event) override;

    private:
        Navigator2D&    _navigator;    /** Reference to the navigator */
    };

    /** Type of zoom margin */
    enum class ZoomMarginType
    {
        AbsoluteScreen,  /** Absolute screen pixels */
        RelativeToData   /** Relative to data percentage */
    };

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
     * Get the world zoom rectangle
     * @return Zoom rectangle in world coordinates
     */
    QRectF getZoomRectangleWorld() const;

    /**
     * Set the world zoom rectangle to \p zoomRectangleWorld
     * @param zoomRectangleWorld Zoom rectangle in world coordinates
     */
    void setZoomRectangleWorld(const QRectF& zoomRectangleWorld);

    /**
     * Get the zoom rectangle margin type
     * @return Zoom rectangle margin type
     */
    ZoomMarginType getZoomMarginType() const;

    /**
     * Set the zoom rectangle margin type to \p zoomMarginType
     * @param zoomMarginType Zoom rectangle margin type
     */
    void setZoomMarginType(ZoomMarginType zoomMarginType);

    /**
     * Get the zoom rectangle margin
     * @return Zoom rectangle margin in screen coordinates 
     */
    float getZoomMarginScreen() const;

    /**
     * Set the zoom margin in screen coordinates to \p zoomMarginScreen
     * @param zoomMarginScreen Zoom margin in screen coordinates
     */
    void setZoomMarginScreen(float zoomMarginScreen);

    /**
     * Get the zoom rectangle margin
     * @return Zoom rectangle margin in data coordinates
     */
    float getZoomMarginData() const;

    /**
     * Set the zoom margin in data coordinates to \p zoomMarginData
     * @param zoomMarginData Zoom margin in data coordinates
     */
    void setZoomMarginData(float zoomMarginData);

    /**
     * Get the zoom factor
     * @return Zoom factor
     */
    float getZoomFactor() const;

    /**
     * Set the zoom factor to \p zoomFactor
     * @param zoomFactor Zoom factor
     */
    void setZoomFactor(float zoomFactor);

    /**
     * Get the zoom percentage
     * @return Zoom percentage
     */
    float getZoomPercentage() const;

    /**
     * Set the zoom percentage to \p zoomPercentage
     * @param zoomPercentage Zoom percentage
     */
    void setZoomPercentage(float zoomPercentage);

    /**
     * Get whether the navigator is enabled
     * @return Boolean determining whether the navigator is enabled
     */
    bool isEnabled() const;

    /**
     * Set enabled to \p enabled
     * @param enabled Boolean determining whether the navigator is enabled
     */
    void setEnabled(bool enabled);

    /**
     * Get the navigation action
     * @return Reference to the navigation action
     */
    gui::NavigationAction& getNavigationAction();

    /**
     * Get the navigation action
     * @return Reference to the navigation action
     */
    const gui::NavigationAction& getNavigationAction() const;

    /**
     * Get the source widget device pixel ratio
     * @return Device pixel ratio
     */
    qreal getDevicePixelRatio() const;

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

    /**
     * Set the zoom center in world coordinates to \p zoomCenterWorld
     * @param zoomCenterWorld Zoom center in world coordinates
     */
    void setZoomCenterWorld(const QPointF& zoomCenterWorld);

    /**
     * Reset the view
     * @param force Force reset event when the user has navigated
     */
    void resetView(bool force = false);

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

    /**
     * Get whether the user has navigated
     * @return Boolean determining whether the user has navigated
     */
    bool hasUserNavigated() const;

    /**
     * Get the zoom region rectangle in screen coordinates
     * @return Zoom region rectangle in screen coordinates
     */
    QRect getZoomRegionRectangle() const;

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

    /** Begin changing the zoom rectangle in world coordinates */
    void beginChangeZoomRectangleWorld();

    /** End changing the zoom rectangle in world coordinates */
    void endChangeZoomRectangleWorld();

    /** Begin zooming to a region */
    void beginZoomToRegion();

    /** End zooming to a region */
    void endZoomToRegion();

    /** Begin zoom margin change */
    void beginChangeZoomMargin();

    /** End zoom margin change */
    void endChangeZoomMargin();

protected: // Cursor

    /**
     * Change the cursor to \p cursor
     * @param cursor Cursor
     */
    void changeCursor(const QCursor& cursor);

    /**  Restore cached cursor */
    void restoreCursor() const;

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

    /**
     * Signals that enabled changed to \p enabled
     * @param enabled Boolean determining whether the navigator is enabled
     */
    void enabledChanged(bool enabled);

    /**
     * Signals that the zoom rectangle in world coordinates has changed from \p previousZoomRectangleWorld to \p currentZoomRectangleWorld
     * @param previousZoomRectangleWorld Previous world zoom rectangle
     * @param currentZoomRectangleWorld Current world zoom rectangle
     */
    void zoomRectangleWorldChanged(const QRectF& previousZoomRectangleWorld, const QRectF& currentZoomRectangleWorld);

    /**
     * Signals that the zoom center in world coordinates has changed from \p previousZoomCenterWorld to \p currentZoomCenterWorld
     * @param previousZoomCenterWorld Previous world zoom center
     * @param currentZoomCenterWorld Current world zoom center
     */
    void zoomCenterWorldChanged(const QPointF& previousZoomCenterWorld, const QPointF& currentZoomCenterWorld);

    /**
     * Signals that the zoom factor has changed from \p previousZoomFactor to \p currentZoomFactor
     * @param previousZoomFactor Previous zoom factor
     * @param currentZoomFactor Current zoom factor
     */
    void zoomFactorChanged(float previousZoomFactor, float currentZoomFactor);

    /** Signals that the zoom margin is about to chang */
    void aboutToChangeZoomMargin();

    /** Signals that the zoom margin has changed */
    void zoomMarginChanged();

private:
    QPointer<QWidget>               _sourceWidget;                          /** Source widget for panning and zooming */
    Renderer2D&                     _renderer;                              /** Reference to parent renderer */
    bool                            _enabled;                               /** Enabled flag */
    bool                            _initialized;                           /** Initialized flag */
    QVector<QPoint>                 _mousePositions;                        /** Recorded mouse positions */
    bool                            _isNavigating;                          /** Navigating flag */
    bool                            _isPanning;                             /** Panning flag */
    bool                            _isZooming;                             /** Zooming flag */
    float                           _zoomFactor;                            /** Zoom factor */
    QPointF                         _zoomCenterWorld;                       /** Zoom rectangle top-left in world coordinates */
    ZoomMarginType                  _zoomMarginType;                        /** Zoom margin type */
    float                           _zoomMarginScreen;                      /** Zoom margin in screen space (pixels) */
    float                           _zoomMarginData;                        /** Zoom margin in relative data space (percentage) */
    QVector<QPoint>                 _zoomRegionPoints;                      /** Zoom region points */
    QRect                           _zoomRegionRectangle;                   /** Zoom region rectangle */
    bool                            _zoomRegionInProgress;                  /** Zoom region in progress flag */
    QRectF                          _previousZoomRectangleWorld;            /** Previous world zoom rectangle */
    bool                            _userHasNavigated;                      /** Boolean determining whether the user has navigated */
    gui::NavigationAction           _navigationAction;                      /** Navigation group action */
    QCursor                         _cachedCursor;                          /** Cached cursor */
    QPointer<ZoomOverlayWidget>     _zoomOverlayWidget;                     /** Zoom overlay widget */
    bool                            _prohibitZoomFactorValueChange;         /** Prohibit zoom factor value change flag, used to prevent infinite loops when setting the zoom factor from the outside */
    bool                            _prohibitZoomPercentageValueChange;     /** Prohibit zoom percentage value change flag, used to prevent infinite loops when setting the zoom percentage from the outside */
};

}
