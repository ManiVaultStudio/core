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

Renderer2D::Renderer2D(QWidget* sourceWidget, QObject* parent) :
    Renderer(parent),
    _sourceWidget(sourceWidget),
    _isNavigating(false),
    _isPanning(false),
    _isZooming(false)
{
    if (_sourceWidget) {
        _sourceWidget->installEventFilter(this);
        _sourceWidget->setFocusPolicy(Qt::StrongFocus);
    }
}

bool Renderer2D::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == QEvent::KeyPress) {
        if (const auto* keyEvent = dynamic_cast<QKeyEvent*>(event)) {
	        if (keyEvent->key() == Qt::Key_Alt) {
		        beginNavigation();

                return Renderer::eventFilter(watched, event);
	        }
        }
    }

    if (event->type() == QEvent::KeyRelease) {
        if (const auto* keyEvent = dynamic_cast<QKeyEvent*>(event)) {
	        if (keyEvent->key() == Qt::Key_Alt) {
		        endNavigation();

                return Renderer::eventFilter(watched, event);
	        }
        }
    }

    if (isNavigating()) {

    	if (event->type() == QEvent::Wheel) {
            if (auto* wheelEvent = dynamic_cast<QWheelEvent*>(event))
                zoomAround(wheelEvent->position().toPoint(), static_cast<float>(wheelEvent->angleDelta().x()) / 1200.f);
        }

        if (event->type() == QEvent::MouseButtonPress) {
            if (const auto* mouseEvent = dynamic_cast<QMouseEvent*>(event))
            {
                if (mouseEvent->button() == Qt::MiddleButton)
                    resetView();

                if (mouseEvent->buttons() == Qt::LeftButton)
                {
                    _sourceWidget->setCursor(Qt::ClosedHandCursor);

                    _mousePositions << mouseEvent->pos();

	                _sourceWidget->update();
                }
            }
        }

        if (event->type() == QEvent::MouseButtonRelease) {
            _sourceWidget->setCursor(Qt::ArrowCursor);

            _mousePositions.clear();

			_sourceWidget->update();
        }

        if (event->type() == QEvent::MouseMove) {
            if (const auto* mouseEvent = dynamic_cast<QMouseEvent*>(event))
            {
                _mousePositions << mouseEvent->pos();

                if (mouseEvent->buttons() == Qt::LeftButton && _mousePositions.size() >= 2)
                {
                    const auto& previousMousePosition   = _mousePositions[_mousePositions.size() - 2];
                    const auto& currentMousePosition    = _mousePositions[_mousePositions.size() - 1];
                    const auto panVector                = currentMousePosition - previousMousePosition;

                    panBy(panVector);
                }
            }
        }
    }

	return Renderer::eventFilter(watched, event);
}

void Renderer2D::zoomAround(const QPointF& center, float factor)
{
#ifdef RENDERER_2D_VERBOSE
    qDebug() << __FUNCSIG__ << center << factor;
#endif

    beginZooming();
    {
        
    }
    endZooming();

    // _navigationAction.getZoomRectangleAction().setBounds(_dataRectangleAction.getBounds());
}

void Renderer2D::zoomToRectangle(const QRectF& zoomRectangle)
{
#ifdef RENDERER_2D_VERBOSE
    qDebug() << __FUNCSIG__ << zoomRectangle;
#endif

    beginZooming();
    {

    }
    endZooming();

    //auto& zoomRectangleAction = _navigationAction.getZoomRectangleAction();

    //const auto moveBy = QPointF(to.x() / _widgetSizeInfo.width * zoomRectangleAction.getWidth() * _widgetSizeInfo.ratioWidth * -1.f,
    //    to.y() / _widgetSizeInfo.height * zoomRectangleAction.getHeight() * _widgetSizeInfo.ratioHeight);

    //zoomRectangleAction.translateBy({ moveBy.x(), moveBy.y() });

    //update();
}

void Renderer2D::panBy(const QPointF& to)
{
#ifdef RENDERER_2D_VERBOSE
    qDebug() << __FUNCSIG__ << to;
#endif

    beginPanning();
    {
	    
    }
    endPanning();

    //auto& zoomRectangleAction = _navigationAction.getZoomRectangleAction();

    //// the widget might have a different aspect ratio than the square opengl viewport
    //const auto offsetBounds = QPointF(zoomRectangleAction.getWidth() * (0.5f * (1 - _widgetSizeInfo.ratioWidth)),
    //    zoomRectangleAction.getHeight() * (0.5f * (1 - _widgetSizeInfo.ratioHeight)) * -1.f);

    //const auto originBounds = QPointF(zoomRectangleAction.getLeft(), zoomRectangleAction.getTop());

    //// translate mouse point in widget to mouse point in bounds coordinates
    //const auto atTransformed = QPointF(at.x() / _widgetSizeInfo.width * zoomRectangleAction.getWidth() * _widgetSizeInfo.ratioWidth,
    //    at.y() / _widgetSizeInfo.height * zoomRectangleAction.getHeight() * _widgetSizeInfo.ratioHeight * -1.f);

    //const auto atInBounds = originBounds + offsetBounds + atTransformed;

    //// ensure mouse position is the same after zooming
    //const auto currentBoundCenter = zoomRectangleAction.getCenter();

    //float moveMouseX = (atInBounds.x() - currentBoundCenter.first) * factor;
    //float moveMouseY = (atInBounds.y() - currentBoundCenter.second) * factor;

    //// zoom and move view
    //zoomRectangleAction.translateBy({ moveMouseX, moveMouseY });
    //zoomRectangleAction.expandBy(-1.f * factor);

    //update();
}

void Renderer2D::resetView()
{
#ifdef RENDERER_2D_VERBOSE
    qDebug() << __FUNCSIG__;
#endif
}

bool Renderer2D::isPanning() const
{
    return _isPanning;
}

bool Renderer2D::isZooming() const
{
    return _isZooming;
}

bool Renderer2D::isNavigating() const
{
	return _isNavigating;
}

void Renderer2D::setIsPanning(bool isPanning)
{
    if (isPanning == _isPanning)
        return;

    _isPanning = isPanning;

    emit isPanningChanged(_isPanning);
}

void Renderer2D::setIsZooming(bool isZooming)
{
    if (isZooming == _isZooming)
        return;

    _isZooming = isZooming;

    emit isZoomingChanged(_isZooming);
}

void Renderer2D::setIsNavigating(bool isNavigating)
{
    if (isNavigating == _isNavigating)
        return;

    _isNavigating = isNavigating;

    emit isNavigatingChanged(_isNavigating);
}

void Renderer2D::beginPanning()
{
#ifdef RENDERER_2D_VERBOSE
    qDebug() << __FUNCSIG__;
#endif

    setIsPanning(true);

    emit panningStarted();
}

void Renderer2D::endPanning()
{
#ifdef RENDERER_2D_VERBOSE
    qDebug() << __FUNCSIG__;
#endif

    setIsPanning(false);

    emit panningEnded();
}

void Renderer2D::beginZooming()
{
#ifdef RENDERER_2D_VERBOSE
    qDebug() << __FUNCSIG__;
#endif

    setIsZooming(true);

    emit zoomingStarted();
}

void Renderer2D::endZooming()
{
#ifdef RENDERER_2D_VERBOSE
    qDebug() << __FUNCSIG__;
#endif

    setIsZooming(false);

    emit zoomingEnded();
}

void Renderer2D::beginNavigation()
{
#ifdef RENDERER_2D_VERBOSE
    qDebug() << __FUNCSIG__;
#endif

    setIsNavigating(true);

    emit navigationStarted();
}

void Renderer2D::endNavigation()
{
#ifdef RENDERER_2D_VERBOSE
    qDebug() << __FUNCSIG__;
#endif

    setIsNavigating(false);

    emit navigationEnded();
}

}
