// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Navigator2D.h"

#include "Renderer2D.h"

#include <QPainter>

#ifdef _DEBUG
    //#define NAVIGATOR_2D_VERBOSE
#endif

//#define NAVIGATOR_2D_VERBOSE

using namespace mv::gui;

namespace mv
{

Navigator2D::ZoomOverlayWidget::ZoomOverlayWidget(Navigator2D& navigator, QWidget* targetWidget):
	gui::OverlayWidget(targetWidget),
	_navigator(navigator)
{
}

void Navigator2D::ZoomOverlayWidget::paintEvent(QPaintEvent* event)
{
    if (_navigator.getZoomRegionRectangle().isValid()) {
        QPainter painter(this);

        painter.setBrush(QColor(0, 0, 0, 50));
        painter.setPen(QPen(QColor(0, 0, 0, 150), 2));
        painter.drawRect(_navigator.getZoomRegionRectangle());

        event->accept();
    }
}

Navigator2D::Navigator2D(Renderer2D& renderer, QObject* parent) :
    QObject(parent),
    _renderer(renderer),
    _enabled(false),
    _initialized(false),
    _isNavigating(false),
    _isPanning(false),
    _isZooming(false),
    _zoomFactor(1.0f),
    _zoomMarginScreen(100.f),
    _zoomMarginWorld(.0f),
    _zoomRegionInProgress(false),
    _userHasNavigated(),
    _navigationAction(this, "Navigation")
{
}

void Navigator2D::initialize(QWidget* sourceWidget)
{
    Q_ASSERT(sourceWidget);

    if (!sourceWidget)
        return;

    _sourceWidget = sourceWidget;

    _sourceWidget->installEventFilter(this);
    _sourceWidget->setFocusPolicy(Qt::StrongFocus);

    connect(&getNavigationAction().getZoomExtentsAction(), &TriggerAction::triggered, this, [this]() -> void {
        resetView(true);
    });

    const auto zoomRectangleChanged = [this]() -> void {
        setZoomRectangleWorld(_navigationAction.getZoomRectangleAction().toRectF());
    };

    zoomRectangleChanged();

    connect(&_navigationAction.getZoomRectangleAction(), &DecimalRectangleAction::rectangleChanged, this, zoomRectangleChanged);

    connect(this, &Navigator2D::zoomRectangleWorldChanged, this, [this, zoomRectangleChanged](const QRectF& previousZoomRectangleWorld, const QRectF& currentZoomRectangleWorld) -> void {
        disconnect(&_navigationAction.getZoomRectangleAction(), &DecimalRectangleAction::rectangleChanged, this, nullptr);
        {
            _navigationAction.getZoomExtentsAction().setEnabled(hasUserNavigated());

            _navigationAction.getZoomRectangleAction().setRectangle(currentZoomRectangleWorld.left(), currentZoomRectangleWorld.right(), currentZoomRectangleWorld.bottom(), currentZoomRectangleWorld.top());
        }
        connect(&_navigationAction.getZoomRectangleAction(), &DecimalRectangleAction::rectangleChanged, this, zoomRectangleChanged);

        _navigationAction.getZoomPercentageAction().setValue(getZoomPercentage());
	});

    connect(this, &Navigator2D::zoomCenterWorldChanged, this, [this](const QPointF& previousZoomCenterWorld, const QPointF& currentZoomCenterWorld) -> void {
        _navigationAction.getZoomCenterAction().set(currentZoomCenterWorld);
	});

    connect(this, &Navigator2D::zoomFactorChanged, this, [this](float previousZoomFactor, float currentZoomFactor) -> void {
        _navigationAction.getZoomFactorAction().setValue(currentZoomFactor);
	});

    connect(&_navigationAction.getZoomCenterAction(), &DecimalPointAction::valueChanged, this, [this](float x, float y) -> void {
        beginChangeZoomRectangleWorld();
        {
            setZoomCenterWorld(QPointF(x, y));
        }
        endChangeZoomRectangleWorld();
	});

    connect(&_navigationAction.getZoomInAction(), &TriggerAction::triggered, this, [this]() -> void {
        setZoomPercentage(getZoomPercentage() + 10.f);
    });

    connect(&_navigationAction.getZoomPercentageAction(), &DecimalAction::valueChanged, this, [this](float value) -> void {
        _userHasNavigated = true;
        setZoomPercentage(value);
	});

    connect(&_navigationAction.getZoomOutAction(), &TriggerAction::triggered, this, [this]() -> void {
        setZoomPercentage(getZoomPercentage() - 10.f);
	});

    connect(&_navigationAction.getZoomFactorAction(), &DecimalAction::valueChanged, this, [this](float value) -> void {
        setZoomFactor(value);
	});

    connect(&_renderer, &Renderer2D::worldBoundsChanged, this, [this](const QRectF& worldBounds) -> void {
        if (!hasUserNavigated())
            resetView();
	});

    connect(&_navigationAction.getZoomRegionAction(), &TriggerAction::triggered, this, [this]() -> void {
        beginZoomToRegion();
	});

    setZoomFactor(_navigationAction.getZoomFactorAction().getValue());

    _sourceWidget->addAction(&_navigationAction.getZoomInAction());
    _sourceWidget->addAction(&_navigationAction.getZoomExtentsAction());
    _sourceWidget->addAction(&_navigationAction.getZoomOutAction());
    _sourceWidget->addAction(&_navigationAction.getZoomSelectionAction());
    _sourceWidget->addAction(&_navigationAction.getZoomRegionAction());

    _zoomOverlayWidget = new ZoomOverlayWidget(*this, _sourceWidget);

    _initialized = true;
}

bool Navigator2D::eventFilter(QObject* watched, QEvent* event)
{
    if (!_initialized || !_enabled)
        return false;

    if (getNavigationAction().getFreezeNavigation().isChecked())
        return QObject::eventFilter(watched, event);

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
        if (_zoomRegionInProgress) {
            const auto updateZoomRegion = [this]() -> void {
                int x1 = std::min(_zoomRegionPoints[0].x(), _zoomRegionPoints[1].x());
                int y1 = std::min(_zoomRegionPoints[0].y(), _zoomRegionPoints[1].y());
                int x2 = std::max(_zoomRegionPoints[0].x(), _zoomRegionPoints[1].x());
                int y2 = std::max(_zoomRegionPoints[0].y(), _zoomRegionPoints[1].y());

				_zoomRegionRectangle = QRect(x1, y1, x2 - x1, y2 - y1);

                if (_zoomOverlayWidget)
                    _zoomOverlayWidget->update();
			};

            if (event->type() == QEvent::MouseButtonPress) {
                if (const auto* mouseEvent = dynamic_cast<QMouseEvent*>(event)) {
                    if (mouseEvent->buttons() == Qt::LeftButton) {
                        _zoomRegionPoints << mouseEvent->pos() << mouseEvent->pos();

                        updateZoomRegion();
                    }
                }
            }

            if (event->type() == QEvent::MouseMove) {
                if (const auto* mouseEvent = dynamic_cast<QMouseEvent*>(event)) {
                    if (mouseEvent->buttons() == Qt::LeftButton) {
                        if (_zoomRegionPoints.size() == 2)
                            _zoomRegionPoints[1] = mouseEvent->pos();

                        updateZoomRegion();
                    }
                }
            }

            if (event->type() == QEvent::MouseButtonRelease) {
                if (const auto* mouseEvent = dynamic_cast<QMouseEvent*>(event)) {
                    if (mouseEvent->button() == Qt::LeftButton) {
                        endZoomToRegion();
                    }
                }
            }
        } else {
            if (event->type() == QEvent::Wheel) {
                if (auto* wheelEvent = dynamic_cast<QWheelEvent*>(event)) {
                    constexpr auto zoomSensitivity = .1f;

                    if (wheelEvent->angleDelta().x() < 0)
                        zoomAround(wheelEvent->position().toPoint(), 1.0f - zoomSensitivity);
                    else
                        zoomAround(wheelEvent->position().toPoint(), 1.0f + zoomSensitivity);
                }
            }

            if (event->type() == QEvent::MouseButtonPress) {
                if (const auto* mouseEvent = dynamic_cast<QMouseEvent*>(event)) {
                    if (mouseEvent->button() == Qt::MiddleButton)
                        resetView();

                    if (mouseEvent->buttons() == Qt::LeftButton) {
                        changeCursor(Qt::ClosedHandCursor);

                        _mousePositions << mouseEvent->pos();

                        _sourceWidget->update();
                    }
                }
            }

            if (event->type() == QEvent::MouseButtonRelease) {
                restoreCursor();

                _mousePositions.clear();

                _sourceWidget->update();
            }

            if (event->type() == QEvent::MouseMove) {
                if (const auto* mouseEvent = dynamic_cast<QMouseEvent*>(event)) {
                    _mousePositions << mouseEvent->pos();

                    if (mouseEvent->buttons() == Qt::LeftButton && _mousePositions.size() >= 2) {
                        const auto& previousMousePosition   = _mousePositions[_mousePositions.size() - 2];
                        const auto& currentMousePosition    = _mousePositions[_mousePositions.size() - 1];
                        const auto panVector                = currentMousePosition - previousMousePosition;

                        panBy(-panVector);
                    }
                }
            }
        }
    }

    return QObject::eventFilter(watched, event);
}

QMatrix4x4 Navigator2D::getViewMatrix() const
{
    QMatrix4x4 lookAt, scale;

    const auto zoomRectangle = getZoomRectangleWorld();

    // Construct look-at parameters
    const auto eye      = QVector3D(zoomRectangle.center().x(), zoomRectangle.center().y(), 1);
    const auto center   = QVector3D(zoomRectangle.center().x(), zoomRectangle.center().y(), 0);
    const auto up       = QVector3D(0, 1, 0);

#ifdef NAVIGATOR_2D_VERBOSE
    qDebug() << __FUNCTION__ << getZoomRectangleWorld() << _renderer.getDataBounds() << eye;
#endif

    // Create look-at transformation matrix
    lookAt.lookAt(eye, center, up);

    const auto viewerSize   = _renderer.getRenderSize();
    const auto factorX      = static_cast<float>(viewerSize.width()) / (zoomRectangle.isValid() ? static_cast<float>(zoomRectangle.width()) : 1.0f);
    const auto factorY      = static_cast<float>(viewerSize.height()) / (zoomRectangle.isValid() ? static_cast<float>(zoomRectangle.height()) : 1.0f);
    const auto scaleFactor  = factorX < factorY ? factorX : factorY;

    const auto d = 1.0f - (2 * 0) / std::max(viewerSize.width(), viewerSize.height());

    // Create scale matrix
    scale.scale(scaleFactor * d, scaleFactor * d, scaleFactor * d);

    // Return composite matrix of scale and look-at transformation matrix
    return scale * lookAt;
}

QRectF Navigator2D::getZoomRectangleWorld() const
{
    const auto zoomRectangleWorldSize       = _renderer.getRenderSize().toSizeF() * _zoomFactor;
    const auto zoomRectangleWorldTopLeft    = _zoomCenterWorld - QPointF(.5f * static_cast<float>(zoomRectangleWorldSize.width()), .5f * static_cast<float>(zoomRectangleWorldSize.height()));

    return {
        zoomRectangleWorldTopLeft,
        zoomRectangleWorldSize
    };
}

void Navigator2D::setZoomRectangleWorld(const QRectF& zoomRectangleWorld)
{
    if (getNavigationAction().getFreezeNavigation().isChecked())
        return;

#ifdef NAVIGATOR_2D_VERBOSE
    qDebug() << __FUNCTION__ << zoomRectangleWorld;
#endif

    const auto previousZoomRectangleWorld = getZoomRectangleWorld();

    if (zoomRectangleWorld == previousZoomRectangleWorld)
        return;

    _zoomFactor = std::max(
        static_cast<float>(zoomRectangleWorld.width()) / static_cast<float>(_renderer.getRenderSize().width()),
        static_cast<float>(zoomRectangleWorld.height()) / static_cast<float>(_renderer.getRenderSize().height())
    );

    _zoomCenterWorld = zoomRectangleWorld.center();

	emit zoomRectangleWorldChanged(previousZoomRectangleWorld, getZoomRectangleWorld());
}

float Navigator2D::getZoomMarginScreen() const
{
    return _zoomMarginScreen;
}

float Navigator2D::getZoomFactor() const
{
    return _zoomFactor;
}

void Navigator2D::setZoomFactor(float zoomFactor)
{
    if (getNavigationAction().getFreezeNavigation().isChecked())
        return;

    if (zoomFactor == _zoomFactor)
        return;

    const auto previousZoomFactor = _zoomFactor;

    _zoomFactor = zoomFactor;

	emit zoomFactorChanged(previousZoomFactor, _zoomFactor);

    setZoomPercentage(getZoomPercentage());
}

float Navigator2D::getZoomPercentage() const
{
    const auto worldBounds          = _renderer.getWorldBounds();
    const auto zoomRectangleWorld   = getZoomRectangleWorld();

    if (!worldBounds.isValid() || !zoomRectangleWorld.isValid())
        return 1.0f;

    const auto factorX      = static_cast<float>(worldBounds.width()) / static_cast<float>(zoomRectangleWorld.width());
    const auto factorY      = static_cast<float>(worldBounds.height()) / static_cast<float>(zoomRectangleWorld.height());
    const auto scaleFactor  = factorX > factorY ? factorX : factorY;

    return scaleFactor * 100.f;
}

void Navigator2D::setZoomPercentage(float zoomPercentage)
{
    if (getNavigationAction().getFreezeNavigation().isChecked())
        return;

    beginZooming();
    {
        beginChangeZoomRectangleWorld();
        {
            if (zoomPercentage < 0.01f)
                return;

            const auto zoomPercentageNormalized = .01f * zoomPercentage;
            const auto zoomFactorX              = static_cast<float>(_renderer.getWorldBounds().width()) / static_cast<float>(_renderer.getRenderSize().width());
            const auto zoomFactorY              = static_cast<float>(_renderer.getWorldBounds().height()) / static_cast<float>(_renderer.getRenderSize().height());

            setZoomFactor(std::max(zoomFactorX, zoomFactorY) / zoomPercentageNormalized);
        }
        endChangeZoomRectangleWorld();
    }
    endZooming();
}

bool Navigator2D::isEnabled() const
{
    return _enabled;
}

void Navigator2D::setEnabled(bool enabled)
{
    if (enabled == _enabled)
        return;

    _enabled = enabled;

    _navigationAction.setShortcutsEnabled(_enabled);

    emit enabledChanged(_enabled);
}

gui::NavigationAction& Navigator2D::getNavigationAction()
{
    return _navigationAction;
}

const gui::NavigationAction& Navigator2D::getNavigationAction() const
{
    return _navigationAction;
}

qreal Navigator2D::getDevicePixelRatio() const
{
    if (_sourceWidget)
        return _sourceWidget->devicePixelRatioF();

    return 1.0;
}

void Navigator2D::zoomAround(const QPoint& center, float factor)
{
    if (getNavigationAction().getFreezeNavigation().isChecked())
        return;

    if (!_initialized)
        return;

    beginZooming();
    {
        beginChangeZoomRectangleWorld();
        {
            const auto p1 = _renderer.getScreenPointToWorldPosition(getViewMatrix(), center).toPointF();

            setZoomFactor(_zoomFactor /= factor);
            setZoomCenterWorld(p1 + (_zoomCenterWorld - p1) / factor);
        }
        endChangeZoomRectangleWorld();
    }
    endZooming();
}

void Navigator2D::zoomToRectangle(const QRectF& zoomRectangle)
{
    if (getNavigationAction().getFreezeNavigation().isChecked())
        return;

	if (!_initialized)
        return;

    beginZooming();
    {
        beginChangeZoomRectangleWorld();
        {
            setZoomRectangleWorld(zoomRectangle);
        }
        endChangeZoomRectangleWorld();
    }
    endZooming();
}

void Navigator2D::panBy(const QPointF& delta)
{
    if (getNavigationAction().getFreezeNavigation().isChecked())
        return;

	if (!_initialized)
        return;

    beginPanning();
    {
        beginChangeZoomRectangleWorld();
        {
            const auto p1 = _renderer.getScreenPointToWorldPosition(getViewMatrix(), QPoint()).toPointF();
            const auto p2 = _renderer.getScreenPointToWorldPosition(getViewMatrix(), delta.toPoint()).toPointF();

            setZoomCenterWorld(getZoomRectangleWorld().center() + (p2 - p1));
        }
        endChangeZoomRectangleWorld();
    }
    endPanning();
}

void Navigator2D::setZoomCenterWorld(const QPointF& zoomCenterWorld)
{
    if (getNavigationAction().getFreezeNavigation().isChecked())
        return;

    if (zoomCenterWorld == _zoomCenterWorld)
        return;

    const auto previousZoomCenterWorld = _zoomCenterWorld;

    _zoomCenterWorld = zoomCenterWorld;

    emit zoomCenterWorldChanged(previousZoomCenterWorld, _zoomCenterWorld);
}

void Navigator2D::resetView(bool force /*= false*/)
{
    if (!force && (mv::projects().isOpeningProject() || mv::projects().isImportingProject()))
        return;

    if (!_initialized)
        return;

    if (!force && hasUserNavigated())
        return;

    if (_navigationAction.getFreezeNavigation().isChecked())
        return;

#ifdef NAVIGATOR_2D_VERBOSE
    qDebug() << __FUNCTION__ << force;
#endif

    beginZooming();
    {
        beginChangeZoomRectangleWorld();
	    {
            const auto zoomFactorX = _renderer.getWorldBounds().width() / static_cast<float>(_renderer.getRenderSize().width());
            const auto zoomFactorY = _renderer.getWorldBounds().height() / static_cast<float>(_renderer.getRenderSize().height());

            setZoomFactor(std::max(zoomFactorX, zoomFactorY));
            setZoomCenterWorld(_renderer.getWorldBounds().center());
        }
        endChangeZoomRectangleWorld();
    }
    endZooming();

    _userHasNavigated = false;
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

bool Navigator2D::hasUserNavigated() const
{
    return _userHasNavigated;
}

QRect Navigator2D::getZoomRegionRectangle() const
{
	return _zoomRegionRectangle;
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

#ifdef NAVIGATOR_2D_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    setIsPanning(true);

    _userHasNavigated = true;

    emit panningStarted();
}

void Navigator2D::endPanning()
{
    if (!_initialized)
        return;

#ifdef NAVIGATOR_2D_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    setIsPanning(false);

    emit panningEnded();
}

void Navigator2D::beginZooming()
{
    if (!_initialized)
        return;

#ifdef NAVIGATOR_2D_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    setIsZooming(true);

    _userHasNavigated = true;

    emit zoomingStarted();
}

void Navigator2D::endZooming()
{
    if (!_initialized)
        return;

#ifdef NAVIGATOR_2D_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    setIsZooming(false);

    emit zoomingEnded();
}

void Navigator2D::beginNavigation()
{
    if (!_initialized)
        return;

#ifdef NAVIGATOR_2D_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    _userHasNavigated = true;

    changeCursor(Qt::OpenHandCursor);

    setIsNavigating(true);

    emit navigationStarted();
}

void Navigator2D::endNavigation()
{
    if (!_initialized)
        return;

#ifdef NAVIGATOR_2D_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    changeCursor(Qt::ArrowCursor);

    setIsNavigating(false);

    emit navigationEnded();
}

void Navigator2D::beginChangeZoomRectangleWorld()
{
    _previousZoomRectangleWorld = getZoomRectangleWorld();
}

void Navigator2D::endChangeZoomRectangleWorld()
{
    emit zoomRectangleWorldChanged(_previousZoomRectangleWorld, getZoomRectangleWorld());
}

void Navigator2D::beginZoomToRegion()
{
    setIsNavigating(true);

    _zoomRegionInProgress   = true;
    _zoomRegionRectangle    = QRect();

    if (_zoomOverlayWidget)
        _zoomOverlayWidget->update();
}

void Navigator2D::endZoomToRegion()
{
    _zoomRegionPoints.clear();

    const auto p1 = _renderer.getScreenPointToWorldPosition(getViewMatrix(), _zoomRegionRectangle.topLeft()).toPointF();
    const auto p2 = _renderer.getScreenPointToWorldPosition(getViewMatrix(), _zoomRegionRectangle.bottomRight()).toPointF();

    setZoomRectangleWorld(QRectF(p1, p2));

	_zoomRegionInProgress   = false;
    _zoomRegionRectangle    = QRect();

    if (_zoomOverlayWidget)
        _zoomOverlayWidget->update();
}

void Navigator2D::changeCursor(const QCursor& cursor)
{
	Q_ASSERT(_sourceWidget);

	if (!_sourceWidget)
		return;

    _cachedCursor = _sourceWidget->cursor();

	_sourceWidget->setCursor(cursor);
}

void Navigator2D::restoreCursor() const
{
	Q_ASSERT(_sourceWidget);

	if (!_sourceWidget)
		return;

    _sourceWidget->setCursor(_cachedCursor);
}
}
