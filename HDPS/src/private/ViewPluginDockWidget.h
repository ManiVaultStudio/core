#pragma once

#include "DockWidget.h"

#include <ViewPlugin.h>

#include <DockManager.h>

#include <QMenu>

/**
 * View plugin dock widget class
 *
 * Dock widget which shows the contents of an HDPS view plugin.
 *
 * @author Thomas Kroes
 */
class ViewPluginDockWidget final : public DockWidget
{
    Q_OBJECT

private:

    /** Widget which contains a settings action widget (purpose is to provide a size hint)*/
    class SettingsActionWidget : public QWidget {
    public:

        /**
         * Construct with \p parent and pointer to \p settingsAction
         * @param parent Pointer to parent widget
         * @param settingsAction Pointer to settingsAction
         */
        SettingsActionWidget(QWidget* parent, hdps::gui::WidgetAction* settingsAction);

        /** Override minimum size hint and derive it from the settings action widget */
        QSize sizeHint() const override;

        /** Override minimum size hint and derive it from the settings action widget */
        QSize minimumSizeHint() const override;

    private:
        hdps::gui::WidgetAction*    _settingsAction;    /** Pointer to settings action to set the size hint for */
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
    ViewPluginDockWidget(const QString& title, hdps::plugin::ViewPlugin* viewPlugin, QWidget* parent = nullptr);

    /**
     * Construct view plugin dock from \p variantMap
     * @param variantMap View plugin variant map
     */
    ViewPluginDockWidget(const QVariantMap& variantMap);

    /** Destructor (needed to update the active dock widgets) */
    ~ViewPluginDockWidget();

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
    hdps::plugin::ViewPlugin* getViewPlugin();

    /** Restores the view plugin state */
    void restoreViewPluginState();

    /** Restores the view plugin states of all active view plugins */
    static void restoreViewPluginStates();

public: // Title bar settings menu

    /**
     * Get settings menu for view plugin dock widget (edit view, create screenshot etc.)
     * @return Pointer to settings menu
     */
    QMenu* getSettingsMenu() override;

public: // Serialization

    /**
     * Load view plugin dock widget from variant
     * @param Variant representation of the view plugin dock widget
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save view plugin dock widget to variant
     * @return Variant representation of the view plugin dock widget
     */
    QVariantMap toVariantMap() const override;

public: // View plugin isolation

    /** Caches the visibility */
    void cacheVisibility();

    /** Restores the visibility */
    void restoreVisibility();

private:

    /**
     * Assign \p viewPlugin to dock widget (configures the dock widget properties and sets the dock widget widget)
     * @param viewPlugin Pointer to view plugin
     */
    void setViewPlugin(hdps::plugin::ViewPlugin* viewPlugin);

private:
    hdps::plugin::ViewPlugin*       _viewPlugin;                /** Pointer to view plugin */
    QString                         _viewPluginKind;            /** Kind of (view) plugin */
    QVariantMap                     _viewPluginMap;             /** View plugin cached map for deferred loading */
    QMenu                           _settingsMenu;              /** Menu for view plugin settings */
    QMenu                           _toggleMenu;                /** Menu for toggling view plugin dock widgets */
    hdps::gui::TriggerAction        _helpAction;                /** Action for triggering help */
    bool                            _cachedVisibility;          /** Cached visibility for view plugin isolation */
    ads::CDockManager               _dockManager;               /** Dock manager for internal docking */
    QMap<QString, CDockWidget*>     _settingsDockWidgetsMap;    /** Created dock widgets for settings actions */

protected:
    static QList<ViewPluginDockWidget*> active;  /** Loaded view plugin dock widgets */

    friend class ViewPluginsDockWidget;
    friend class WorkspaceManager;
};

using ViewPluginDockWidgets = QVector<QPointer<ViewPluginDockWidget>>;
