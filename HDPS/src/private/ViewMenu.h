#pragma once

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
        LoadStandardViewSubMenu     = 0x00001,      /** Show menu for loading standard views */
        LoadView                    = 0x00002,      /** Show menu for loading views */
        LoadedViewsSubMenu          = 0x00004,      /** Show menu for toggling loaded views visibility */

        Default = LoadStandardViewSubMenu | LoadView | LoadedViewsSubMenu
    };

    Q_DECLARE_FLAGS(Options, Option)

public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     * @param options Menu options
     */
    ViewMenu(QWidget *parent = nullptr, const Options& options = Option::Default);

    /**
     * Invoked when the menu is shown
     * @param showEvent Pointer to the show event
     */
    void showEvent(QShowEvent* showEvent) override;

private:
    Options     _options;               /** Menu options */
    QMenu       _standardViewsMenu;     /** Menu for loading standard views */
    QMenu       _loadedViewsMenu;       /** Menu for toggling loaded views */
    QAction*    _separator;             /** Separator */
};
