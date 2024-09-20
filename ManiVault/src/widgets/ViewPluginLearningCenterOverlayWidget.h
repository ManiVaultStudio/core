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
#include "util/Video.h"

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
         * @param iconSize Size of the item icon
         */
        AbstractToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, OverlayWidget* overlayWidget, const QSize& iconSize = QSize(16, 16));

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

        /** Updates the item icon (for badge update) */
        void updateIcon();

        /** Toggles item visibility based on the learning center visibility action status */
        void installVisibilityToggle();

    private:

        /** Update visibility based on current settings */
        void updateVisibility();

    private:
        const plugin::ViewPlugin*   _viewPlugin;            /** Const pointer to source view plugin */
        OverlayWidget*              _overlayWidget;         /** Pointer to overlay widget */
        const QSize                 _iconSize;              /** Size of the item icon */
        QHBoxLayout                 _layout;                /** For placing the icon label */
        QLabel                      _iconLabel;             /** Icon label */
        mv::util::WidgetFader       _widgetFader;           /** For fading in/out */
        bool                        _hasVisibilityToggle;   /** Boolean determining whether a visibility toggle is installed */
    };

    /** Toolbar item widget for hiding the other toolbar widget items */
    class VisibleToolbarItemWidget final : public AbstractToolbarItemWidget
    {
    public:

        /**
         * Construct with pointer to \p viewPlugin and \p overlayWidget
         * @param viewPlugin Pointer to view plugin
         * @param overlayWidget Pointer to overlay widget
         */
        VisibleToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, OverlayWidget* overlayWidget);

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

    /** Toolbar widget to align items horizontally or vertically */
    class ToolbarWidget : public QWidget {
    public:

        /**
         * Construct with pointer to \p viewPlugin, parent \p overlayWidget  and \p alignment
         * @param viewPlugin Pointer to the view plugin
         * @param overlayWidget Pointer to parent overlay widget
         * @param alignment Item alignment (supported alignment flags: Qt::AlignTop, Qt::AlignBottom, Qt::AlignLeft, Qt::AlignRight, Qt::AlignCenter)
         * @param alwaysVisible Whether the toolbar widget should always be visible, regardless of the view plugin overlay visibility setting
         */
        ToolbarWidget(const plugin::ViewPlugin* viewPlugin, OverlayWidget* overlayWidget, const Qt::Alignment& alignment, bool alwaysVisible = false);

        /**
         * Add \p widget to the toolbar
         * @param widget Pointer to widget to add
         */
        void addWidget(QWidget* widget);

    private:

        /**
         * Respond to \p target events
         * @param target Object of which an event occurred
         * @param event The event that took place
         */
        bool eventFilter(QObject* target, QEvent* event) override;

    private:
        const plugin::ViewPlugin*   _viewPlugin;        /** Const pointer to source view plugin */
        OverlayWidget*              _overlayWidget;     /** Pointer to owning overlay widget */
        QBoxLayout*                 _layout;            /** For alignment of items */
    };

public:

    /**
     * Construct with pointer to \p target widget, \p viewPlugin and initial \p alignment
     * @param target Pointer to parent widget (overlay widget will be layered on top of this widget)
     * @param viewPlugin Pointer to the view plugin for which to create the overlay
     * @param alignment Alignment w.r.t. to the \p source widget
     */
    ViewPluginLearningCenterOverlayWidget(QWidget* target, const plugin::ViewPlugin* viewPlugin, const Qt::Alignment& alignment = Qt::AlignBottom);

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

    /**
     * Set layout contents margins to { \p margin, \p margin, \p margin, \p margin }
     * @param margin Contents margins
     */
    void setContentsMargins(std::int32_t margin);

    /** Updates the background gradient */
    void updateBackgroundStyle();

private:
    const plugin::ViewPlugin*   _viewPlugin;                        /** Pointer to the view plugin for which to create the overlay */
    const Qt::Alignment         _alignment;                         /** Alignment w.r.t. to the source widget (supported alignment flags: Qt::AlignTop, Qt::AlignBottom, Qt::AlignLeft, Qt::AlignRight, Qt::AlignCenter) */
    QHBoxLayout                 _layout;                            /** For alignment of the learning center popup widget */
    QBoxLayout*                 _toolbarsLayout;                    /** Layout for the settings and actions toolbars */
    QVBoxLayout                 _verticalToolbarLayout;             /** Vertical toolbar for the top and bottom alignment */
    ToolbarWidget               _settingsToolbarWidget;             /** Toolbar widget for learning center settings such as the visibility */
    ToolbarWidget               _actionsToolbarWidget;              /** Toolbar widget which contains the various learning center actions */
    OverlayWidget               _backgroundOverlayWidget;           /** Widget with background content */
    mv::util::WidgetFader       _backgroundOverlayWidgetFader;      /** For fading in/out the background overlay widget */
};

}
