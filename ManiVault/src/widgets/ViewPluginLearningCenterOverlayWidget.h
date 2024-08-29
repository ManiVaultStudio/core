// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "widgets/OverlayWidget.h"
#include "widgets/IconLabel.h"

#include "util/WidgetFader.h"

#include <QHBoxLayout>
#include <QMouseEvent>

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
class CORE_EXPORT ViewPluginLearningCenterOverlayWidget : public OverlayWidget
{
private:

    class ToolbarItemWidget : public QWidget {
    public:
        ToolbarItemWidget(const QIcon& icon);

        void enterEvent(QEnterEvent* event) override;
        void leaveEvent(QEvent* event) override;

    private:
        QHBoxLayout                 _layout;            /** For alignment of the icon label */
        QLabel                      _iconLabel;         /** Icon label */
        mv::util::WidgetFader       _widgetFader;       /** For fading in/out */
    };

    class ToolbarWidget : public QWidget {
    public:
        ToolbarWidget(const plugin::ViewPlugin* viewPlugin, OverlayWidget* overlayWidget);

        void mousePressEvent(QMouseEvent* event) override;

        /**
         * Respond to \p target events
         * @param target Object of which an event occurred
         * @param event The event that took place
         */
        bool eventFilter(QObject* target, QEvent* event) override;

    private:

        /**
         * Get view plugin learning center context menu
         * @return View plugin learning center context menu
         */
        QMenu* getContextMenu(QWidget* parent = nullptr) const;

        /**
         * Set layout contents margins to { \p margin, \p margin, \p margin, \p margin }
         * @param margin Contents margins
         */
        void setContentsMargins(std::int32_t margin);

    private:
        const plugin::ViewPlugin*   _viewPlugin;        /** Const pointer to source view plugin */
        OverlayWidget*              _overlayWidget;     /** Pointer to owning overlay widget */
        QHBoxLayout                 _layout;            /** For alignment of the icon label */
        QLabel                      _iconLabel;         /** Icon label */
        mv::util::WidgetFader       _widgetFader;       /** For fading in/out */
    };

public:

    /**
     * Construct with pointer to \p target widget, \p viewPlugin and initial \p alignment
     * @param target Pointer to parent widget (overlay widget will be layered on top of this widget)
     * @param viewPlugin Pointer to the view plugin for which to create the overlay
     * @param alignment Alignment w.r.t. to the \p source widget
     */
    ViewPluginLearningCenterOverlayWidget(QWidget* target, const plugin::ViewPlugin* viewPlugin, const Qt::Alignment& alignment = Qt::AlignBottom | Qt::AlignRight);

    /**
     * Set target widget to \p targetWidget
     * @param targetWidget Pointer to target widget
     */
    void setTargetWidget(QWidget* targetWidget);

private:

    /**
     * Set layout contents margins to { \p margin, \p margin, \p margin, \p margin }
     * @param margin Contents margins
     */
    void setContentsMargins(std::int32_t margin);

private:
    const plugin::ViewPlugin*   _viewPlugin;        /** Pointer to the view plugin for which to create the overlay */
    const Qt::Alignment&        _alignment;         /** Alignment w.r.t. to the source widget */
    QHBoxLayout                 _layout;            /** For alignment of the learning center popup widget */
    ToolbarWidget               _toolbarWidget;     /** Toolbar widget which contains the actions */
};

}
