// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "DockWidget.h"

#include <ViewPlugin.h>
#include <Task.h>

#include <actions/TaskAction.h>

#include <widgets/OverlayWidget.h>

#include <DockManager.h>

#include <QMenu>
#include <QMap>

class DockManager;

/**
 * View plugin dock widget class
 *
 * Dock widget which shows the contents of an ManiVault view plugin.
 *
 * @author Thomas Kroes
 */
class ViewPluginDockWidget final : public DockWidget
{
    Q_OBJECT

private:

    /** Widget which contains a settings action widget (purpose is to provide a size hint) */
    class SettingsActionWidget : public QWidget {
    public:

        /**
         * Construct with \p parent and pointer to \p settingsAction
         * @param parent Pointer to parent widget
         * @param settingsAction Pointer to settingsAction
         */
        SettingsActionWidget(QWidget* parent, mv::gui::WidgetAction* settingsAction);

        /** Override minimum size hint and derive it from the settings action widget */
        QSize sizeHint() const override;

        /** Override minimum size hint and derive it from the settings action widget */
        QSize minimumSizeHint() const override;

    private:
        mv::gui::WidgetAction*    _settingsAction;    /** Pointer to settings action to set the size hint for */
    };

    /** Overlay widget which shows a very thin view progress bar */
    class ProgressOverlayWidget : public mv::gui::OverlayWidget {
    public:

        /**
         * Construct with pointer to \p parent widget
         * @param parent Pointer to parent widget
         */
        ProgressOverlayWidget(QWidget* parent);

        /**
         * Set task to \p task
         * @param task Pointer to task (maybe nullptr)
         */
        void setTask(mv::Task* task);

    protected:
        /**
         * Override QObject's event handling
         * @return Boolean Whether the event was recognized and processed
         */
        bool event(QEvent* event) override;

    private:

        /** Update the overlay visibility based on whether there is a task and, if yes, whether it is running or not */
        void updateVisibility();

        /** Respond to changes in application palette */
        void updateCustomStyle();

    private:
        mv::gui::TaskAction     _loadTaskAction;    /** Action for reporting loading progress */
        QWidget*                _loadTaskWidget;    /** Pointer to task widget created by ProgressOverlayWidget#_loadTaskAction */
        mv::Task*               _task;              /** Pointer to task */
    };

public:

    /**
     * Construct view plugin dock from \p title and \p parent widget
     * @param title Title of the dock widget
     * @param parent Pointer to parent widget
     */
    ViewPluginDockWidget(const QString& title = "", QWidget* parent = nullptr);

    /**
     * Construct view plugin dock from \p title, \p viewPlugin and \p parent widget
     * @param title Title of the dock widget
     * @param viewPlugin Pointer to view plugin
     * @param parent Pointer to parent widget
     */
    ViewPluginDockWidget(const QString& title, mv::plugin::ViewPlugin* viewPlugin, QWidget* parent = nullptr);

    /**
     * Construct view plugin dock from \p variantMap
     * @param variantMap View plugin variant map
     */
    ViewPluginDockWidget(const QVariantMap& variantMap);

    /** Destructor (needed to update the active dock widgets) */
    ~ViewPluginDockWidget() override;

    /**
     * Get string that describes the dock widget type
     * @return Type string
     */
    QString getTypeString() const override;

    /** Performs custom view plugin dock widget initialization */
    void initialize() override;

    /** Load view plugin */
    void loadViewPlugin();

    /**
     * Get the view plugin
     * @return Pointer to view plugin (might be nullptr)
     */
    mv::plugin::ViewPlugin* getViewPlugin() const;

    /** Restores the view plugin state */
    void restoreViewPluginState();

public: // Title bar settings menu

    /**
     * Get settings menu for view plugin dock widget (edit view, create screenshot etc.)
     * @return Pointer to settings menu
     */
    QMenu* getSettingsMenu() override;

public: // Serialization

    /**
     * Load view plugin dock widget from variant
     * @param variantMap Variant representation of the view plugin dock widget
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save view plugin dock widget to variant
     * @return Variant representation of the view plugin dock widget
     */
    QVariantMap toVariantMap() const override;

    /**
     * Create a view plugin dock widget serialization task for \p viewPluginDockWidgetId (even if no instance is yet available)
     * This is needed by the workspace manager for proper progress reporting
     * @param parent Pointer to parent object
     * @param viewPluginDockWidgetId Globally unique identifier of the view plugin dock widget
     * @param dockManager Pointer to owning dock manager instance
     */
    static void preRegisterSerializationTask(QObject* parent, const QString& viewPluginDockWidgetId, DockManager* dockManager);

    /**
     * Get serialization task for \p viewPluginDockWidgetId
     * @param viewPluginDockWidgetId Globally unique identifier of the view plugin dock widget
     * @return Pointer to task, nullptr if not found
     */
    static mv::Task* getSerializationTask(const QString& viewPluginDockWidgetId);

    /** Removes all tasks which where created with ViewPluginDockWidget::preRegisterSerializationTask(...) */
    static void removeAllSerializationTasks();

public: // View plugin isolation

    /** Caches the visibility */
    void cacheVisibility();

    /** Restores the visibility */
    void restoreVisibility();

private:

    /**
     * Assign \p viewPlugin to dock widget (configures the dock widget properties and sets the dock widget)
     * @param viewPlugin Pointer to view plugin
     */
    void setViewPlugin(mv::plugin::ViewPlugin* viewPlugin);

private:
    QPointer<mv::plugin::ViewPlugin>    _viewPlugin;                /** Pointer to view plugin */
    QString                             _viewPluginKind;            /** Kind of (view) plugin */
    QVariantMap                         _viewPluginMap;             /** View plugin cached map for deferred loading */
    QMenu                               _settingsMenu;              /** Menu for view plugin settings */
    QMenu                               _toggleMenu;                /** Menu for toggling view plugin dock widgets */
    mv::gui::TriggerAction              _helpAction;                /** Action for triggering help */
    bool                                _cachedVisibility;          /** Cached visibility for view plugin isolation */
    CDockWidget                         _centralDockWidget;         /** Dock manager central widget */
    ads::CDockManager                   _dockManager;               /** Dock manager for internal docking */
    QMap<QString, CDockWidget*>         _settingsDockWidgetsMap;    /** Created dock widgets for settings actions */
    //mv::Task                            _progressTask;              /** Task for reporting dock widget progress */
    ProgressOverlayWidget               _progressOverlayWidget;     /** Overlay widget which shows a very thin view progress bar */

protected:

    /**
     * Map view plugin dock widget identifier to serialization task
     * A serialization task is there to report progress during ViewPluginDockWidget::fromVariantMap() and ViewPluginDockWidget::toVariantMap()
     */
    static QMap<QString, mv::Task*>  serializationTasks;

    friend class ViewPluginsDockWidget;
    friend class WorkspaceManager;
};
