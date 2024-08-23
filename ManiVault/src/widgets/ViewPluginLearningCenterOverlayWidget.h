// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "widgets/OverlayWidget.h"
#include "util/WidgetFader.h"

#include <QHBoxLayout>
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
class CORE_EXPORT ViewPluginLearningCenterOverlayWidget : public OverlayWidget
{
private:

    class PopupWidget : public QWidget {
    public:
        PopupWidget(const plugin::ViewPlugin* viewPlugin, QWidget* parent = nullptr);

        void showEvent(QShowEvent* event) override;

        /**
         * Set layout contents margins to { \p margin, \p margin, \p margin, \p margin }
         * @param margin Contents margins
         */
        void setContentsMargins(std::int32_t margin);

    private:
        const plugin::ViewPlugin*   _viewPlugin;    /** Const pointer to source view plugin */
        QHBoxLayout                 _layout;        /** For alignment of the icon label */
        std::vector<QLabel>         _labels;        /** Labels */
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
     * Respond to target object events
     * @param target Object of which an event occurred
     * @param event The event that took place
     * @return Boolean determining whether the event was handled or not
     */
    bool eventFilter(QObject* target, QEvent* event) override;

    /**
     * Set target widget to \p targetWidget
     * @param targetWidget Pointer to target widget
     */
    void setTargetWidget(QWidget* targetWidget);

    void showEvent(QShowEvent* event) override;

public: // Ignore mouse events for only this overlay widget

    //void mouseMoveEvent(QMouseEvent* event) override { event->ignore(); };
    //void mousePressEvent(QMouseEvent* event) override { event->ignore(); };
    //void mouseReleaseEvent(QMouseEvent* event) override { event->ignore(); };
    //void mouseDoubleClickEvent(QMouseEvent* event) override { event->ignore(); };
    //void wheelEvent(QWheelEvent* event) override { event->ignore(); };
    //void enterEvent(QEnterEvent* event) override { event->ignore(); };
    //void leaveEvent(QEvent* event) override { event->ignore(); };

private:

    /**
     * Set layout contents margins to { \p margin, \p margin, \p margin, \p margin }
     * @param margin Contents margins
     */
    void setContentsMargins(std::int32_t margin);

private:
    const plugin::ViewPlugin*   _viewPlugin;        /** Pointer to the view plugin for which to create the overlay */
    const Qt::Alignment&        _alignment;         /** Alignment w.r.t. to the source widget */
    util::WidgetFader           _widgetFader;       /** For fading in on view plugin mouse enter and fading out on view plugin widget mouse leave*/
    QHBoxLayout                 _layout;            /** For alignment of the learning center popup widget */
    PopupWidget                 _popupWidget;       /** Icon with popup */
};

}
