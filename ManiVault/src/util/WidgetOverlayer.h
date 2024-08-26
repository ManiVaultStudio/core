// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QObject>

class QWidget;

namespace mv::util {

/**
 * Widget overlayer utility class
 *
 * Helper class for layering a widget on top of another widget and synchronizing its geometry
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT WidgetOverlayer : public QObject
{
    Q_OBJECT

public:

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
    QWidget* getSourceWidget();

    /**
     * Get target widget
     * @return Pointer to target widget
     */
    QWidget* getTargetWidget();

    /**
     * Set target widget to \p targetWidget
     * @param targetWidget Pointer to target widget
     */
    void setTargetWidget(QWidget* targetWidget);

    /**
     * Respond to \p target events
     * @param target Object of which an event occurred
     * @param event The event that took place
     */
    bool eventFilter(QObject* target, QEvent* event) override;

private:

    /** Source widget geometry is synchronized with the target widget geometry */
    void synchronizeGeometry() const;

private:
    QWidget*    _sourceWidget;      /** Pointer to source widget (will be layered on top of the \p targetWidget) */
    QWidget*    _targetWidget;      /** Pointer to target widget */
};

}
