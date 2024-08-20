// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "widgets/OverlayWidget.h"

#include "util/WidgetFader.h"
#include "util/WidgetOverlayer.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

namespace mv::plugin {
    class ViewPlugin;
}

namespace mv::gui
{

/**
 * View plugin learning center overlay widget class
 *
 * Overlays the source widget with a view plugin learning center icon popup
 *  
 * @author Thomas Kroes
 */
class ViewPluginLearningCenterOverlayWidget : public OverlayWidget
{
private:

    class PopupWidget : public QWidget {
    public:
        PopupWidget(QWidget* parent = nullptr);

    private:
        QLabel  _label;
    };

public:

    /**
     * Construct with pointer to \p source widget and pointer to \p viewPlugin
     * @param source Pointer to source widget
     * @param viewPlugin Pointer to the view plugin for which to create the overlay
     * @param alignment Alignment w.r.t. to the \p source widget
     */
    ViewPluginLearningCenterOverlayWidget(QWidget* source, const plugin::ViewPlugin* viewPlugin, const Qt::Alignment& alignment = Qt::AlignBottom | Qt::AlignRight);

    /**
     * Set target widget to \p targetWidget
     * @param targetWidget Pointer to target widget
     */
    void setTargetWidget(QWidget* targetWidget);

private:
    const plugin::ViewPlugin*   _viewPlugin;        /** Pointer to the view plugin for which to create the overlay */
    const Qt::Alignment&        _alignment;         /** Alignment w.r.t. to the source widget */
    QHBoxLayout                 _layout;            /** For alignment of the learning center icon */
    PopupWidget                 _popupWidget;       /** Icon with popup */
};

}
