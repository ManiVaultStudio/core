#pragma once

#include <QMenu>

/**
 * Load system menu class
 * 
 * Menu class for loading system view plugins
 * 
 * @author Thomas Kroes
 */
class LoadSystemViewMenu : public QMenu
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     */
    LoadSystemViewMenu(QWidget *parent = nullptr);

    /**
     * Invoked when the menu is shown
     * @param showEvent Pointer to the show event
     */
    void showEvent(QShowEvent* showEvent) override;
};
