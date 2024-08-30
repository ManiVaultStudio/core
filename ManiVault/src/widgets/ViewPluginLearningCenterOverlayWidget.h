// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "widgets/OverlayWidget.h"
#include "widgets/IconLabel.h"

#include "util/WidgetFader.h"

#include <QHBoxLayout>
#include <QMouseEvent>

#include "ViewPlugin.h"

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

    /** Base class for toolbar item widgets */
    class AbstractToolbarItemWidget : public QWidget {
    public:

        /**
         * Construct with pointer to \p overlayWidget and \p icon
         * @param viewPlugin Pointer to view plugin
         * @param overlayWidget Pointer to overlay widget
         */
        AbstractToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, OverlayWidget* overlayWidget);

        /**
         * Invoked when the widget is shown
         * @param event Pointer to show event
         */
        void showEvent(QShowEvent* event) override;

        /**
         * Invoked when the mouse enters the toolbar item widget
         * @param event Pointer to enter event
         */
        void enterEvent(QEnterEvent* event) override;

        /**
         * Invoked when the mouse leaves the toolbar item widget
         * @param event Pointer to event
         */
        void leaveEvent(QEvent* event) override;

        /**
         * Get icon
         * @return Icon
         */
        virtual QIcon getIcon() const = 0;

        /**
         * Determine whether the item should be visible or not
         * @return Boolean determining whether the item should be visible or not
         */
        virtual bool shouldDisplay() const = 0;

    protected:

        /**
         * Get view plugin
         * @return Pointer to view plugin
         */
        const plugin::ViewPlugin* getViewPlugin() const;

        /**
         * Get non-const pointer to view plugin
         * @return Non-const pointer to view plugin
         */
        plugin::ViewPlugin* getViewPlugin();

        /**
         * Get non-const pointer to overlay widget
         * @return Non-const pointer to overlay widget
         */
        OverlayWidget* getOverlayWidget() const;

        /**
         * Respond to \p target events
         * @param target Object of which an event occurred
         * @param event The event that took place
         */
        bool eventFilter(QObject* target, QEvent* event) override;

    private:

        /** Updates the item icon (for badge update) */
        void updateIcon();

    private:
        const plugin::ViewPlugin*   _viewPlugin;        /** Const pointer to source view plugin */
        OverlayWidget*              _overlayWidget;     /** Pointer to overlay widget */
        QHBoxLayout                 _layout;            /** For placing the icon label */
        QLabel                      _iconLabel;         /** Icon label */
        mv::util::WidgetFader       _widgetFader;       /** For fading in/out */
    };

    /** Toolbar item widget for hiding the toolbar widget */
    class CloseToolbarItemWidget final : public AbstractToolbarItemWidget
    {
    public:

        /** No need for custom constructor */
        using AbstractToolbarItemWidget::AbstractToolbarItemWidget;

        /**
         * Invoked when the mouse button is pressed
         * @param event Pointer to mouse event
         */
        void mousePressEvent(QMouseEvent* event) override;

        /**
         * Get icon
         * @return Icon
         */
        QIcon getIcon() const override;

        /**
         * Determine whether the item should be visible or not
         * @return Boolean determining whether the item should be visible or not
         */
        bool shouldDisplay() const override;
    };

    /** Toolbar item widget for showing the view plugin related videos */
    class VideosToolbarItemWidget final : public AbstractToolbarItemWidget
    {
    public:

        /**
         * Construct with pointer to \p viewPlugin and \p overlayWidget
         * @param viewPlugin Pointer to view plugin
         * @param overlayWidget Pointer to overlay widget
         */
        VideosToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, OverlayWidget* overlayWidget);

        /**
         * Invoked when the mouse button is pressed
         * @param event Pointer to mouse event
         */
        void mousePressEvent(QMouseEvent* event) override;

        /**
         * Get icon
         * @return Icon
         */
        QIcon getIcon() const override;

        /**
         * Determine whether the item should be visible or not
         * @return Boolean determining whether the item should be visible or not
         */
        bool shouldDisplay() const override;
    };

    /** Toolbar item widget for showing the view plugin description */
    class DescriptionToolbarItemWidget final : public AbstractToolbarItemWidget
    {
    public:

        /**
         * Construct with pointer to \p viewPlugin and \p overlayWidget
         * @param viewPlugin Pointer to view plugin
         * @param overlayWidget Pointer to overlay widget
         */
        DescriptionToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, OverlayWidget* overlayWidget);

        /**
         * Invoked when the mouse button is pressed
         * @param event Pointer to mouse event
         */
        void mousePressEvent(QMouseEvent* event) override;

        /**
         * Get icon
         * @return Icon
         */
        QIcon getIcon() const override;

        /**
         * Determine whether the item should be visible or not
         * @return Boolean determining whether the item should be visible or not
         */
        bool shouldDisplay() const override;
    };

    /** Toolbar item widget for showing the documentation */
    class ShowDocumentationToolbarItemWidget final : public AbstractToolbarItemWidget
    {
    public:

        /**
         * Construct with pointer to \p viewPlugin and \p overlayWidget
         * @param viewPlugin Pointer to view plugin
         * @param overlayWidget Pointer to overlay widget
         */
        ShowDocumentationToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, OverlayWidget* overlayWidget);

        /**
         * Invoked when the mouse button is pressed
         * @param event Pointer to mouse event
         */
        void mousePressEvent(QMouseEvent* event) override;

        /**
         * Get icon
         * @return Icon
         */
        QIcon getIcon() const override;

        /**
         * Determine whether the item should be visible or not
         * @return Boolean determining whether the item should be visible or not
         */
        bool shouldDisplay() const override;
    };

    /** Toolbar item widget for showing the shortcut map */
    class ShortcutsToolbarItemWidget final : public AbstractToolbarItemWidget
    {
    public:

        /**
         * Construct with pointer to \p viewPlugin and \p overlayWidget
         * @param viewPlugin Pointer to view plugin
         * @param overlayWidget Pointer to overlay widget
         */
        ShortcutsToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, OverlayWidget* overlayWidget);

        /**
         * Invoked when the mouse button is pressed
         * @param event Pointer to mouse event
         */
        void mousePressEvent(QMouseEvent* event) override;

        /**
         * Get icon
         * @return Icon
         */
        QIcon getIcon() const override;

        /**
         * Determine whether the item should be visible or not
         * @return Boolean determining whether the item should be visible or not
         */
        bool shouldDisplay() const override;
    };

    /** Toolbar item widget for visiting the Github repository website */
    class VisitGithubRepoToolbarItemWidget final : public AbstractToolbarItemWidget
    {
    public:

        /**
         * Construct with pointer to \p viewPlugin and \p overlayWidget
         * @param viewPlugin Pointer to view plugin
         * @param overlayWidget Pointer to overlay widget
         */
        VisitGithubRepoToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, OverlayWidget* overlayWidget);

        /**
         * Invoked when the mouse button is pressed
         * @param event Pointer to mouse event
         */
        void mousePressEvent(QMouseEvent* event) override;

        /**
         * Get icon
         * @return Icon
         */
        QIcon getIcon() const override;

        /**
         * Determine whether the item should be visible or not
         * @return Boolean determining whether the item should be visible or not
         */
        bool shouldDisplay() const override;
    };

    /** Toolbar item widget for visiting the global learning center */
    class ToLearningCenterToolbarItemWidget final : public AbstractToolbarItemWidget
    {
    public:

        /**
         * Construct with pointer to \p viewPlugin and \p overlayWidget
         * @param viewPlugin Pointer to view plugin
         * @param overlayWidget Pointer to overlay widget
         */
        ToLearningCenterToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, OverlayWidget* overlayWidget);

        /**
         * Invoked when the mouse button is pressed
         * @param event Pointer to mouse event
         */
        void mousePressEvent(QMouseEvent* event) override;

        /**
         * Get icon
         * @return Icon
         */
        QIcon getIcon() const override;

        /**
         * Determine whether the item should be visible or not
         * @return Boolean determining whether the item should be visible or not
         */
        bool shouldDisplay() const override;

    private:
        
    };

    class ToolbarWidget : public QWidget {
    public:
        ToolbarWidget(const plugin::ViewPlugin* viewPlugin, OverlayWidget* overlayWidget);

    private:

        /**
         * Respond to \p target events
         * @param target Object of which an event occurred
         * @param event The event that took place
         */
        bool eventFilter(QObject* target, QEvent* event) override;

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
