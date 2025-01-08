// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QPointer>

class QWidget;

namespace mv::util {

/**
 * Widget overlayer utility class
 *
 * Helper class for layering a widget on top of another widget and synchronizing its geometry.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT WidgetOverlayer : public QObject
{
    Q_OBJECT

public:

    using MouseEventReceiverWidgets = QVector<const QWidget*>;

    /**
     * Construct widget overlayer with pointer to \p parent object, \p sourceWidget, \p targetWidget and \p initialOpacity
     * @param parent Pointer to parent object
     * @param sourceWidget Pointer to source widget (will be layered on top of the \p targetWidget)
     * @param targetWidget Pointer to target widget (\p sourceWidget will be layered on top of it)
     * @param initialOpacity Opacity of \p sourceWidget at initialization
     */
    WidgetOverlayer(QObject* parent, QWidget* sourceWidget, QWidget* targetWidget, float initialOpacity = 1.0f);

    /**
     * Get source widget
     * @return Pointer to source widget
     */
    QWidget* getSourceWidget() const;

    /**
     * Get target widget
     * @return Pointer to target widget
     */
    QWidget* getTargetWidget() const;

    /**
     * Set target widget to \p targetWidget
     * @param targetWidget Pointer to target widget
     */
    void setTargetWidget(QWidget* targetWidget);

    /**
     * Add \p mouseEventReceiverWidget
     * @param mouseEventReceiverWidget Const pointer to mouse event receiver widget that should be added
     */
    void addMouseEventReceiverWidget(const QWidget* mouseEventReceiverWidget);

    /**
     * Remove \p mouseEventReceiverWidget
     * @param mouseEventReceiverWidget Const pointer to mouse event receiver widget that should be removed
     */
    void removeMouseEventReceiverWidget(const QWidget* mouseEventReceiverWidget);

    /**
     * Get mouse event receiver widgets
     * @return Vector of mouse event receiver widgets
     */
    MouseEventReceiverWidgets getMouseEventReceiverWidgets();

    /**
     * Determine the source widget should receive mouse events or not
     * @return Boolean determining whether the mouse cursor is under any of the mouse event receiver widgets
     */
    bool shouldReceiveMouseEvents() const;

    /**
     * Respond to \p target events
     * @param target Object of which an event occurred
     * @param event The event that took place
     */
    bool eventFilter(QObject* target, QEvent* event) override;

private:

    /** Source widget geometry is synchronized with the target widget geometry */
    void synchronizeGeometry() const;

signals:

    /**
     * Signals that the target widget changed from \p previousTargetWidget to \p currentTargetWidget
     * @param previousTargetWidget Pointer to previous target widget
     * @param currentTargetWidget Pointer to current target widget
     */
    void targetWidgetChanged(QWidget* previousTargetWidget, QWidget* currentTargetWidget);

    /**
     * Signals that \p mouseEventReceiverWidget is added
     * @param mouseEventReceiverWidget Pointer to mouse receiver widget
     */
    void mouseEventReceiverWidgetAdded(const QWidget* mouseEventReceiverWidget);

    /**
     * Signals that \p mouseEventReceiverWidget is removed
     * @param mouseEventReceiverWidget Pointer to mouse receiver widget
     */
    void mouseEventReceiverWidgetRemoved(const QWidget* mouseEventReceiverWidget);

private:
    QPointer<QWidget>           _sourceWidget;                  /** Pointer to source widget (will be layered on top of the \p targetWidget) */
    QPointer<QWidget>           _targetWidget;                  /** Pointer to target widget */
    MouseEventReceiverWidgets   _mouseEventReceiverWidgets;     /** Child widgets of the source widget that should receive mouse events, despite te \p Qt::WA_TransparentForMouseEvents attribute of the source widget */
};

}
