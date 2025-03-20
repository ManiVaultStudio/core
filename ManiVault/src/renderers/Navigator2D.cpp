// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Navigator2D.h"
#include "Renderer2D.h"

#ifdef _DEBUG
    //#define NAVIGATOR_2D_VERBOSE
#endif

#define NAVIGATOR_2D_VERBOSE

namespace mv
{

Navigator2D::Navigator2D(QObject* parent) :
    QObject(parent),
    _initialized(false),
    _isNavigating(false),
    _isPanning(false),
    _isZooming(false)
{
}

void Navigator2D::initialize(QWidget* sourceWidget, Renderer2D* renderer)
{
    Q_ASSERT(sourceWidget && renderer);

    if (sourceWidget && renderer) {
        _sourceWidget = sourceWidget;
        _renderer = renderer;

        _sourceWidget->installEventFilter(this);
        _sourceWidget->setFocusPolicy(Qt::StrongFocus);

        _initialized = true;
    }
}

bool Navigator2D::eventFilter(QObject* watched, QEvent* event)
{
    if (!_initialized)
        return false;

    if (event->type() == QEvent::KeyPress) {
        if (const auto* keyEvent = dynamic_cast<QKeyEvent*>(event)) {
            if (keyEvent->key() == Qt::Key_Alt) {
                beginNavigation();

                return QObject::eventFilter(watched, event);
            }
        }
    }

    if (event->type() == QEvent::KeyRelease) {
        if (const auto* keyEvent = dynamic_cast<QKeyEvent*>(event)) {
            if (keyEvent->key() == Qt::Key_Alt) {
                endNavigation();

                return QObject::eventFilter(watched, event);
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
                    const auto& previousMousePosition = _mousePositions[_mousePositions.size() - 2];
                    const auto& currentMousePosition = _mousePositions[_mousePositions.size() - 1];
                    const auto panVector = currentMousePosition - previousMousePosition;

                    panBy(panVector);
                }
            }
        }
    }

    return QObject::eventFilter(watched, event);
}

void Navigator2D::zoomAround(const QPointF& center, float factor)
{
    if (!_initialized)
        return;

#ifdef RENDERER_2D_VERBOSE
    qDebug() << __FUNCTION__ << center << factor;
#endif

    beginZooming();
    {

    }
    endZooming();

    // _navigationAction.getZoomRectangleAction().setBounds(_dataRectangleAction.getBounds());
}

void Navigator2D::zoomToRectangle(const QRectF& zoomRectangle)
{
    if (!_initialized)
        return;

#ifdef RENDERER_2D_VERBOSE
    qDebug() << __FUNCTION__ << zoomRectangle;
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

void Navigator2D::panBy(const QPointF& to)
{
    if (!_initialized)
        return;

#ifdef RENDERER_2D_VERBOSE
    qDebug() << __FUNCTION__ << to;
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

void Navigator2D::resetView()
{
    if (!_initialized)
        return;

#ifdef RENDERER_2D_VERBOSE
    qDebug() << __FUNCTION__;
#endif
}

bool Navigator2D::isPanning() const
{
    return _isPanning;
}

bool Navigator2D::isZooming() const
{
    return _isZooming;
}

bool Navigator2D::isNavigating() const
{
    return _isNavigating;
}

void Navigator2D::setIsPanning(bool isPanning)
{
    if (!_initialized)
        return;

    if (isPanning == _isPanning)
        return;

    _isPanning = isPanning;

    emit isPanningChanged(_isPanning);
}

void Navigator2D::setIsZooming(bool isZooming)
{
    if (!_initialized)
        return;

    if (isZooming == _isZooming)
        return;

    _isZooming = isZooming;

    emit isZoomingChanged(_isZooming);
}

void Navigator2D::setIsNavigating(bool isNavigating)
{
    if (!_initialized)
        return;

    if (isNavigating == _isNavigating)
        return;

    _isNavigating = isNavigating;

    emit isNavigatingChanged(_isNavigating);
}

void Navigator2D::beginPanning()
{
    if (!_initialized)
        return;

#ifdef RENDERER_2D_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    setIsPanning(true);

    emit panningStarted();
}

void Navigator2D::endPanning()
{
    if (!_initialized)
        return;

#ifdef RENDERER_2D_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    setIsPanning(false);

    emit panningEnded();
}

void Navigator2D::beginZooming()
{
    if (!_initialized)
        return;

#ifdef RENDERER_2D_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    setIsZooming(true);

    emit zoomingStarted();
}

void Navigator2D::endZooming()
{
    if (!_initialized)
        return;

#ifdef RENDERER_2D_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    setIsZooming(false);

    emit zoomingEnded();
}

void Navigator2D::beginNavigation()
{
    if (!_initialized)
        return;

#ifdef RENDERER_2D_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    setIsNavigating(true);

    emit navigationStarted();
}

void Navigator2D::endNavigation()
{
    if (!_initialized)
        return;

#ifdef RENDERER_2D_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    setIsNavigating(false);

    emit navigationEnded();
}

}
