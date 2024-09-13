// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "util/WidgetOverlayer.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QWidget>

#include <functional>

namespace mv::plugin {
    class PluginFactory;
}

/**
 * Learning page plugin action widget class
 *
 * Widget class which shows the plugin name and some additional actions (launch read me etc.)
 *
 * @author Thomas Kroes
 */
class LearningPagePluginActionsWidget : public QWidget
{
private:

    /** Visual representation of a plugin action */
    class ActionWidget : public QLabel {
    public:
        using ClickedFunction = std::function<void()>;

        /**
         * Create with \p iconName, \p clickedFunction and pointer to \p parent widget
         * @param iconName Action icon name
         * @param clickedFunction Function that is invoked when the action is clicked
         * @param iconName Action icon name
         */
        explicit ActionWidget(const QString& iconName, ClickedFunction clickedFunction, QWidget* parent = nullptr);

        /**
         * Invoked on mouse press
         * @param event Pointer to mouse event
         */
        void mousePressEvent(QMouseEvent* event) override;

        /**
         * Invoked on mouse hover
         * @param enterEvent Pointer to enter event
         */
        void enterEvent(QEnterEvent* enterEvent) override;

        /**
         * Triggered on mouse leave
         * @param leaveEvent Pointer to leave event
         */
        void leaveEvent(QEvent* leaveEvent) override;

    private:

        /** Updates the CSS styling */
        void updateStyle();

    private:
        const QString       _iconName;          /** Action icon name */
        ClickedFunction     _clickedFunction;   /** Function that is invoked when the action is clicked */
    };

    class ActionsOverlayWidget : public QWidget {
    public:
        explicit ActionsOverlayWidget(LearningPagePluginActionsWidget* learningPagePluginActionWidget);

    private:
        LearningPagePluginActionsWidget*    _learningPagePluginActionWidget;    /** Pointer to owning learning page plugin actions widget */
        QHBoxLayout                         _mainLayout;                        /** Main vertical layout */
        mv::util::WidgetOverlayer           _widgetOverlayer;                   /** Synchronizes the size with the source widget */
    };

protected:

    /**
     * Construct with pointer to \p pluginFactory and pointer to \p parent widget
     * @param pluginFactory Pointer plugin factory
     * @param parent Pointer parent widget
     */
    LearningPagePluginActionsWidget(const mv::plugin::PluginFactory* pluginFactory, QWidget* parent = nullptr);

    /**
     * Triggered on mouse hover
     * @param enterEvent Pointer to enter event
     */
    void enterEvent(QEnterEvent* enterEvent) override;

    /**
     * Triggered on mouse leave
     * @param leaveEvent Pointer to leave event
     */
    void leaveEvent(QEvent* leaveEvent) override;

    /**
     * Override QObject's event handling
     * @return Boolean Wheter the event was recognized and processed
     */
    bool event(QEvent* event) override;

private:

    /**
     * Get the plugin factory which is associated with the plugin action
     * @param Pointer to plugin factory
     */
    const mv::plugin::PluginFactory* getPluginFactory() const;

    bool hasOverlay() const;

    void updateStyle();

private:
    const mv::plugin::PluginFactory*    _pluginFactory;             /** Pointer to plugin factory */
    QHBoxLayout                         _mainLayout;                /** Main vertical layout */
    ActionsOverlayWidget                _actionsOverlayWidget;      /** Overlay widget which shows the associated plugin actions */

    friend class LearningPagePluginResourcesWidget;
};
