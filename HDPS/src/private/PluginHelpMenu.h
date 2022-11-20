#pragma once

#include <QMenu>

/**
 * Plugins help menu class
 * 
 * Menu class for accessing plugin-specific documentation
 * 
 * @author Thomas Kroes
 */
class PluginHelpMenu : public QMenu
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     */
    PluginHelpMenu(QWidget *parent = nullptr);

    /**
     * Invoked when the menu is shown
     * @param showEvent Pointer to the show event
     */
    void showEvent(QShowEvent* showEvent) override;
};
