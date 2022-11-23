#pragma once

#include "DockWidget.h"

#include <ViewPlugin.h>

#include <QMenu>

/**
 * Visualization widget class
 *
 * Widget class which contains a docking manager for docking view plugins (non-standard)
 *
 * @author Thomas Kroes
 */
class ViewPluginDockWidget : public DockWidget
{
public:

    /**
     * Constructor
     * @param title Title of the dock widget
     * @param parent Pointer to parent widget
     */
    ViewPluginDockWidget(const QString& title, QWidget* parent = nullptr);

    /**
     * Constructor
     * @param title Title of the dock widget
     * @param viewPlugin Pointer to view plugin
     * @param parent Pointer to parent widget
     */
    ViewPluginDockWidget(const QString& title, hdps::plugin::ViewPlugin* viewPlugin, QWidget* parent = nullptr);

    /**
     * Get string that describes the dock widget type
     * @return Type string
     */
    QString getTypeString() const override;

    /** Load view plugin */
    void loadViewPlugin();

    /**
     * Get the view plugin
     * @return Pointer to view plugin (might be nullptr)
     */
    hdps::plugin::ViewPlugin* getViewPlugin();

    /**
     * Get settings menu for view plugin dock widget (edit view, create screenshot etc.)
     * @return Pointer to settings menu
     */
    QMenu* getSettingsMenu() override;

public: // Serialization

    /**
     * Load widget action from variant
     * @param Variant representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant
     * @return Variant representation of the widget action
     */
    QVariantMap toVariantMap() const override;

private:

    /** Populates the settings menu with actions from the view plugin */
    void initializeSettingsMenu();

private:
    hdps::plugin::ViewPlugin*   _viewPlugin;        /** Pointer to view plugin */
    QString                     _viewPluginKind;    /** Kind of (view) plugin */
    QVariantMap                 _viewPluginMap;     /** View plugin cached map for deferred loading */
    QMenu                       _settingsMenu;      /** Menu for view plugin settings */
    hdps::gui::TriggerAction    _helpAction;        /** Action for triggering help */
};

using ViewPluginDockWidgets = QVector<ViewPluginDockWidget*>;
