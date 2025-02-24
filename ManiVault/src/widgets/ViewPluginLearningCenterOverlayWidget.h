// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "widgets/OverlayWidget.h"
#include "widgets/IconLabel.h"

#include "util/WidgetFader.h"

#include <QHBoxLayout>

namespace mv::plugin {
    class ViewPlugin;
}

namespace mv::gui {
    class PluginLearningCenterAction;
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

Q_OBJECT

protected:

    /** Base class for toolbar item widgets */
    class AbstractToolbarItemWidget : public QWidget {
    public:

        /**
         * Construct with pointer to \p overlayWidget and \p icon
         * @param viewPlugin Pointer to view plugin
         * @param overlayWidget Pointer to overlay widget
         * @param iconSize Size of the item icon
         */
        AbstractToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, ViewPluginLearningCenterOverlayWidget* overlayWidget, const QSize& iconSize = QSize(14, 14));

        /** Show only when required (when AbstractToolbarItemWidget::shouldDisplay() returns true) */
        void showConditionally();

        /**
         * Invoked when the widget is shown
         * @param event Pointer to show event
         */
        void showEvent(QShowEvent* event) override;

        /**
         * Invoked when the widget is hidden
         * @param event Pointer to hide event
         */
        void hideEvent(QHideEvent* event) override;

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

        /** Updates the item icon (for badge update) */
        virtual void updateIcon();

        /**
         * Get widget fader
         * @return Reference to widget fader
         */
        util::WidgetFader& getWidgetFader();

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
        ViewPluginLearningCenterOverlayWidget* getOverlayWidget() const;

    protected:
        const plugin::ViewPlugin*                   _viewPlugin;            /** Const pointer to source view plugin */
        ViewPluginLearningCenterOverlayWidget*      _overlayWidget;         /** Pointer to overlay widget */
        const QSize                                 _iconSize;              /** Size of the item icon */
        QHBoxLayout                                 _layout;                /** For placing the icon label */
        QLabel                                      _iconLabel;             /** Icon label */
        util::WidgetFader                           _widgetFader;           /** For fading in/out */
    };

    /** Toolbar item widget for the learning center */
    class LearningCenterToolbarItemWidget final : public AbstractToolbarItemWidget
    {
    public:

        /**
         * Construct with pointer to \p viewPlugin and \p overlayWidget
         * @param viewPlugin Pointer to view plugin
         * @param overlayWidget Pointer to overlay widget
         */
        LearningCenterToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, ViewPluginLearningCenterOverlayWidget* overlayWidget);

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

        /** Updates the item icon (for badge update) */
        void updateIcon() override;
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
        VideosToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, ViewPluginLearningCenterOverlayWidget* overlayWidget);

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

    /** Toolbar item widget for showing the view plugin tutorials */
    class TutorialsToolbarItemWidget final : public AbstractToolbarItemWidget
    {
    public:

        /**
         * Construct with pointer to \p viewPlugin and \p overlayWidget
         * @param viewPlugin Pointer to view plugin
         * @param overlayWidget Pointer to overlay widget
         */
        TutorialsToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, ViewPluginLearningCenterOverlayWidget* overlayWidget);

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
        ShowDocumentationToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, ViewPluginLearningCenterOverlayWidget* overlayWidget);

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

    /** Toolbar item widget for showing the about information */
    class AboutToolbarItemWidget final : public AbstractToolbarItemWidget
    {
    public:

        /**
         * Construct with pointer to \p viewPlugin and \p overlayWidget
         * @param viewPlugin Pointer to view plugin
         * @param overlayWidget Pointer to overlay widget
         */
        AboutToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, ViewPluginLearningCenterOverlayWidget* overlayWidget);

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
        ShortcutsToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, ViewPluginLearningCenterOverlayWidget* overlayWidget);

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
        VisitGithubRepoToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, ViewPluginLearningCenterOverlayWidget* overlayWidget);

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
        ToLearningCenterToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, ViewPluginLearningCenterOverlayWidget* overlayWidget);

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

    /** Toolbar item widget for hiding the toolbar */
    class HideToolbarItemWidget final : public AbstractToolbarItemWidget
    {
    public:

        /**
         * Construct with pointer to \p viewPlugin and \p overlayWidget
         * @param viewPlugin Pointer to view plugin
         * @param overlayWidget Pointer to overlay widget
         */
        HideToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, ViewPluginLearningCenterOverlayWidget* overlayWidget);

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

    /** Toolbar item widget for configuring the alignment */
    class AlignmentToolbarItemWidget final : public AbstractToolbarItemWidget
    {
        /** Bundle Qt alignment, description and icon */
        struct Alignment {
            Qt::Alignment   _alignment;     /** Alignment */
            QString         _title;         /** Description */
            QIcon           _icon;          /** Icon representation of the alignment */
        };

    public:

        /**
         * Construct with pointer to \p viewPlugin and \p overlayWidget
         * @param viewPlugin Pointer to view plugin
         * @param overlayWidget Pointer to overlay widget
         */
        AlignmentToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, ViewPluginLearningCenterOverlayWidget* overlayWidget);

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
        static std::vector<Alignment> alignments;   /** Allowed alignment */
    };

    /** Toolbar widget to align items horizontally or vertically */
    class ToolbarWidget : public QWidget {
    private:

        /** Overlay widget for background */
        class BackgroundWidget : public QWidget
        {
        public:
            
	        /**
             * Construct with pointer to \p target widget and \p viewPlugin
             * @param target Pointer to target widget
             * @param viewPlugin Pointer to view plugin
             */
            BackgroundWidget(QWidget* target, const plugin::ViewPlugin* viewPlugin);

            
	        /**
             * Transition to \p geometry over time
             * @param geometry Geometry at the end of the animation
             * @param animate Whether to animate the geometry over time
             */
            void transitionGeometry(const QRect& geometry, bool animate = true);

        protected:

	        /**
             * Override paint event to do some custom background painting
             * @param event Pointer to paint event
             */
            void paintEvent(QPaintEvent* event) override;

        private:
            const plugin::ViewPlugin*       _viewPlugin;            /** Pointer to view plugin */
            QPropertyAnimation              _geometryAnimation;     /** Animates the position and size of the background widget */
            QRect                           _previousGeometry;      /** Previous geometry, used as start value for geometry animation */
        };

    public:

        /**
         * Construct with pointer to \p viewPlugin and pointer to parent \p overlayWidget
         * @param viewPlugin Pointer to the view plugin
         * @param overlayWidget Pointer to parent overlay widget
         * @param alwaysVisible Whether the toolbar widget should always be visible, regardless of the view plugin overlay visibility setting
         */
        ToolbarWidget(const plugin::ViewPlugin* viewPlugin, ViewPluginLearningCenterOverlayWidget* overlayWidget, bool alwaysVisible = false);

        /**
         * Add \p widget to the toolbar
         * @param widget Pointer to widget to add
         */
        void addWidget(QWidget* widget);

    protected:

        /**
         * Respond to \p target events
         * @param target Object of which an event occurred
         * @param event The event that took place
         */
        bool eventFilter(QObject* target, QEvent* event) override;

        /**
         * Invoked when the widget is shown
         * @param event Pointer to show event that occurred
         */
        void showEvent(QShowEvent* event) override;

        /** Invoked when the plugin learning center alignment changes */
        void alignmentChanged();

    private:

        /** Synchronizes the background widget geometry with our geometry */
        void synchronizeBackgroundWidgetGeometry();

        /** Invoked when the plugin learning center visibility changed */
        void visibilityChanged();

        /**
         * Invoked when the widget is first shown
         * @param showEvent Pointer to show event
         */
        void firstShowEvent(QShowEvent* showEvent);

        /**
         * Get the learning center action
         * @return Reference to the view plugin learning center action
         */
        PluginLearningCenterAction& getLearningCenterAction() const;

    private:
        const plugin::ViewPlugin*               _viewPlugin;                /** Const pointer to source view plugin */
        ViewPluginLearningCenterOverlayWidget*  _overlayWidget;             /** Pointer to owning overlay widget */
        bool                                    _alwaysVisible;             /** Whether the toolbar widget should always be visible, regardless of the view plugin overlay visibility setting */
        std::vector<QWidget*>                   _widgets;                   /** Registered widgets */
        QHBoxLayout                             _layout;                    /** Main layout */
        BackgroundWidget                        _backgroundWidget;          /** Widget with background content */
        util::WidgetFader                       _backgroundWidgetFader;     /** For fading in/out the background widget */
        std::uint32_t                           _numberOfShowEvents;        /** The number of times the widget has been shown */

    protected:
        static constexpr std::int32_t margin = 4;   /** Overall margin */

        friend class ViewPluginLearningCenterOverlayWidget;
    };

public:

    /**
     * Construct with pointer to \p target widget and pointer to \p viewPlugin
     * @param target Pointer to parent widget (overlay widget will be layered on top of this widget)
     * @param viewPlugin Pointer to the view plugin for which to create the overlay
     */
    ViewPluginLearningCenterOverlayWidget(QWidget* target, const plugin::ViewPlugin* viewPlugin);

    /**
     * Set target widget to \p targetWidget
     * @param targetWidget Pointer to target widget
     */
    void setTargetWidget(QWidget* targetWidget);

    /**
     * Get toolbar widget
     * @return Reference to toolbar widget
     */
    ToolbarWidget& getToolbarWidget();

    /**
     * Respond to \p target events
     * @param target Object of which an event occurred
     * @param event The event that took place
     */
    bool eventFilter(QObject* target, QEvent* event) override;

protected: // Expand/collapse

    /** Collapse all visible items in the toolbar */
    void collapse();

    /** Expand all visible items in the toolbar */
    void expand();
    
	/**
     * Get whether all displayable items in the toolbar are visible
     * @return Boolean determining whether all displayable items in the toolbar are visible
     */
    bool isExpanded();

    /**
     * Get whether all displayable items in the toolbar are hidden
     * @return Boolean determining whether all displayable items in the toolbar are hidden
     */
    bool isCollapsed();
    
    /**
     * Invoked when \p toolbarItemWidget is shown
     * @param toolbarItemWidget Pointer to toolbar item widget which is shown
     */
    void toolbarItemWidgetShown(QWidget* toolbarItemWidget);

    /**
     * Invoked when \p toolbarItemWidget is hidden
     * @param toolbarItemWidget Pointer to toolbar item widget which is hidden
     */
    void toolbarItemWidgetHidden(QWidget* toolbarItemWidget);

private:

    /** Invoked when the plugin learning center alignment changes */
    void alignmentChanged();

    /**
     * Get the learning center action
     * @return Reference to the view plugin learning center action
     */
    PluginLearningCenterAction& getLearningCenterAction() const;

signals:

    /** Signals that the toolbar got expanded */
    void expanded();

    /** Signals that the toolbar got collapsed */
    void collapsed();

private:
    const plugin::ViewPlugin*                   _viewPlugin;                            /** Pointer to the view plugin for which to create the overlay */
    QVBoxLayout                                 _layout;                                /** For alignment of the learning center toolbar */
    ToolbarWidget                               _toolbarWidget;                         /** Toolbar widget which contains the various learning center actions */
    LearningCenterToolbarItemWidget             _learningCenterToolbarItemWidget;       /** Primary learning center toolbar item widget */
    VideosToolbarItemWidget                     _videosToolbarItemWidget;               /** Videos toolbar item widget */
    TutorialsToolbarItemWidget                  _tutorialsToolbarItemWidget;            /** Plugin tutorials toolbar item widget */
    AboutToolbarItemWidget                      _aboutToolbarItemWidget;                /** Plugin about toolbar item widget */
    ShortcutsToolbarItemWidget                  _shortcutsToolbarItemWidget;            /** Plugin shortcuts toolbar item widget */
    ShowDocumentationToolbarItemWidget          _showDocumentationToolbarItemWidget;    /** Documentation toolbar item widget */
    VisitGithubRepoToolbarItemWidget            _visitGithubRepoToolbarItemWidget;      /** Link to GitHub repository toolbar item widget */
    ToLearningCenterToolbarItemWidget           _toLearningCenterToolbarItemWidget;     /** Go the main learning center toolbar item widget */
    HideToolbarItemWidget                       _hideToolbarItemWidget;                 /** Toolbar hide item widget */
    AlignmentToolbarItemWidget                  _alignmentToolbarItemWidget;            /** Toolbar alignment item widget */
    std::vector<AbstractToolbarItemWidget*>     _toolbarItemWidgets;                    /** All toolbar item widgets except the learning center toolbar item widget */

    static constexpr auto animationDuration                 = 300;      /** Overall animation duration */
    static constexpr auto intermediateOpacity               = .3f;      /** Intermediate opacity of items */
    static constexpr auto widgetAsyncUpdateTimerInterval    = 10;       /** Prevent QWidget a-synchronicity problems (like show()) by deferring some operations with a timer (of which this variable determines the interval in ms) */

    friend class AbstractToolbarItemWidget;
    friend class AlignmentToolbarItemWidget;
};

}
