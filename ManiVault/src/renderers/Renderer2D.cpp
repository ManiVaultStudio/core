// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Renderer2D.h"

#ifdef _DEBUG
    //#define RENDERER_2D_VERBOSE
#endif

//#define RENDERER_2D_VERBOSE

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

    setWorldBounds(computeWorldBounds());

    getNavigator().resetView();
}

QSize Renderer2D::getRenderSize() const
{
    return _renderSize;
}

Navigator2D& Renderer2D::getNavigator()
{
    if (_customNavigator) {
        return *_customNavigator;
    }

	return _navigator;
}

const Navigator2D& Renderer2D::getNavigator() const
{
    if (_customNavigator) {
        return *_customNavigator;
    }

    return _navigator;
}

void Renderer2D::setSourceWidget(QWidget* sourceWidget)
{
    getNavigator().initialize(sourceWidget);
}

QPointer<Navigator2D> Renderer2D::getCustomNavigator() const
{
	return _customNavigator;
}

void Renderer2D::setCustomNavigator(const QPointer<Navigator2D>& customNavigator)
{
	_customNavigator = customNavigator;
}

void Renderer2D::beginRender()
{
#ifdef RENDERER_2D_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    const auto dpr = getNavigator().getDevicePixelRatio();

    glViewport(0, 0, static_cast<int>(dpr * _renderSize.width()), static_cast<int>(dpr * _renderSize.height()));

    updateModelViewProjectionMatrix();
}

void Renderer2D::endRender()
{
#ifdef RENDERER_2D_VERBOSE
    qDebug() << __FUNCTION__;
#endif
}

QRectF Renderer2D::getDataBounds() const
{
    return _dataBounds;
}

void Renderer2D::setDataBounds(const QRectF& dataBounds)
{
#ifdef RENDERER_2D_VERBOSE
    qDebug() << __FUNCTION__ << dataBounds;
#endif

    const auto previousDataBounds   = _dataBounds;

    if (dataBounds == _dataBounds)
        return;

    _dataBounds = dataBounds;
    
    emit dataBoundsChanged(previousDataBounds, _dataBounds);

    setWorldBounds(computeWorldBounds());
}

QRectF Renderer2D::getWorldBounds() const
{
    return _worldBounds;
}

void Renderer2D::setWorldBounds(const QRectF& worldBounds)
{
#ifdef RENDERER_2D_VERBOSE
    qDebug() << __FUNCTION__ << _worldBounds;
#endif

    const auto previousWorldBounds = _worldBounds;

    if (worldBounds == _worldBounds)
        return;

    _worldBounds = worldBounds;

    emit worldBoundsChanged(previousWorldBounds, _worldBounds);
}

void Renderer2D::updateModelViewProjectionMatrix()
{
    _modelViewProjectionMatrix = QMatrix4x4(getProjectionMatrix()) * getNavigator().getViewMatrix() * _modelMatrix;
}

QVector3D Renderer2D::getScreenPointToWorldPosition(const QMatrix4x4& modelViewMatrix, const QPoint& screenPoint) const
{
    return QVector3D(screenPoint.x(), getRenderSize().height() - screenPoint.y(), 0).unproject(modelViewMatrix, getProjectionMatrix(), QRect(0, 0, getRenderSize().width(), getRenderSize().height()));
}

QVector2D Renderer2D::getWorldPositionToNormalizedScreenPoint(const QVector3D& position) const
{
    const auto clipSpacePos = getProjectionMatrix() * (getNavigator().getViewMatrix() * QVector4D(position, 1.0));

    return (clipSpacePos.toVector3D() / clipSpacePos.w()).toVector2D();
}

QPoint Renderer2D::getWorldPositionToScreenPoint(const QVector3D& position) const
{
    const auto normalizedScreenPoint    = QVector2D(1.0f, -1.0f) * getWorldPositionToNormalizedScreenPoint(position);
    const auto viewSize                 = QVector2D(getRenderSize().width(), getRenderSize().height());

    return (viewSize * ((QVector2D(1.0f, 1.0f) + normalizedScreenPoint) / 2.0f)).toPoint();
}

QVector2D Renderer2D::getScreenPointToNormalizedScreenPoint(const QVector2D& screenPoint) const
{
    const auto viewSize = QVector2D(getRenderSize().width(), getRenderSize().height());

    return QVector2D(-1.f, -1.f) + 2.f * (QVector2D(screenPoint.x(), getRenderSize().height() - screenPoint.y()) / viewSize);
}

QMatrix4x4 Renderer2D::getScreenToNormalizedScreenMatrix() const
{
    QMatrix4x4 translate, scale;

    translate.translate(-1.0f, -1.0f, 0.0f);
    scale.scale(2.0f / static_cast<float>(getRenderSize().width()), 2.0f / static_cast<float>(getRenderSize().height()), 1.0f);

    return translate * scale;
}

QMatrix4x4 Renderer2D::getNormalizedScreenToScreenMatrix() const
{
    QMatrix4x4 translate, scale;

    const auto size     = QSizeF(getRenderSize());
    const auto halfSize = 0.5f * size;

    scale.scale(halfSize.width(), halfSize.height(), 1.0f);
    translate.translate(size.width(), 1, 0.0f);

    return translate * scale;
}

float Renderer2D::getZoomPercentage() const
{
    const auto factorX      = static_cast<float>(getDataBounds().width()) / static_cast<float>(getNavigator().getZoomRectangleWorld().width());
    const auto factorY      = static_cast<float>(getDataBounds().height()) / static_cast<float>(getNavigator().getZoomRectangleWorld().height());
    const auto scaleFactor  = factorX < factorY ? factorX : factorY;

    return scaleFactor;
}

QMatrix4x4 Renderer2D::getProjectionMatrix() const
{
    // Compute half of the widget size
    const auto halfSize = getRenderSize() / 2;

    QMatrix4x4 matrix;

    // Create an orthogonal transformation matrix
    matrix.ortho(-halfSize.width(), halfSize.width(), -halfSize.height(), halfSize.height(), -1000.0f, +1000.0f);

    return matrix;
}

QRect Renderer2D::getScreenRectangleFromWorldRectangle(const QRectF& worldBoundingRectangle) const
{
    // Compute screen bounding rectangle extremes
    const auto topLeftScreen        = getWorldPositionToScreenPoint(QVector3D(worldBoundingRectangle.bottomLeft()));
    const auto bottomRightScreen    = getWorldPositionToScreenPoint(QVector3D(worldBoundingRectangle.topRight()));

    return {
        topLeftScreen,
        bottomRightScreen
    };
}

QMatrix4x4 Renderer2D::getModelMatrix() const
{
    return _modelMatrix;
}

void Renderer2D::setModelMatrix(const QMatrix4x4& modelMatrix)
{
    _modelMatrix = modelMatrix;
}

QMatrix4x4 Renderer2D::getModelViewProjectionMatrix() const
{
    return _modelViewProjectionMatrix;
}

}
