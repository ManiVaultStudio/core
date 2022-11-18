#pragma once

#include <ViewPlugin.h>

#include <actions/PluginTriggerAction.h>

#include <DockAreaWidget.h>

#include <QMenu>

/**
 * View menu class
 * 
 * Menu class for managing viewing plugins
 * 
 * @author Thomas Kroes
 */
class ViewMenu : public QMenu
{
public:

    /** Describes the menu options */
    enum Option {
        LoadSystemViewPlugins   = 0x00001,      /** Show menu for loading system views */
        LoadViewPlugins         = 0x00002,      /** Show menu for loading views */
        LoadedViewsSubMenu      = 0x00004,      /** Show menu for toggling loaded views visibility */

        Default = LoadSystemViewPlugins | LoadViewPlugins | LoadedViewsSubMenu
    };

    Q_DECLARE_FLAGS(Options, Option)

public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     * @param options Menu options
     * @param dockAreaWidget Pointer to dock area widget to which new view plugins are docked (new view plugins will be docked top-level if nullptr)
     */
    ViewMenu(QWidget *parent = nullptr, const Options& options = Option::Default, ads::CDockAreaWidget* dockAreaWidget = nullptr);

    /**
     * Invoked when the menu is shown
     * @param showEvent Pointer to the show event
     */
    void showEvent(QShowEvent* showEvent) override;

private:

    QVector<QAction*> getLoadViewsActions(const hdps::gui::PluginTriggerActions& pluginTriggerActions, hdps::gui::DockAreaFlag dockArea);
    QVector<QAction*> getLoadSystemViewsActions(const hdps::gui::PluginTriggerActions& pluginTriggerActions, hdps::gui::DockAreaFlag dockArea);

private:
    ads::CDockAreaWidget*       _dockAreaWidget;        /** Pointer to dock area widget to which new view plugins are docked (new view plugins will be docked top-level if nullptr) */
    Options                     _options;               /** Menu options */
    QMenu                       _systemViewsMenu;       /** Menu for loading system views */
    QMenu                       _loadedViewsMenu;       /** Menu for toggling loaded views */
    QAction*                    _separator;             /** Separator */
};
