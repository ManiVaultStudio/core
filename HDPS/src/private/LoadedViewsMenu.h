#pragma once

#include <QMenu>

/**
 * Loaded views menu class
 * 
 * Menu class for toggling loaded view plugins visibility
 * 
 * @author Thomas Kroes
 */
class LoadedViewsMenu : public QMenu
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     */
    LoadedViewsMenu(QWidget *parent = nullptr);

    /**
     * Invoked when the menu is shown
     * @param showEvent Pointer to the show event
     */
    void showEvent(QShowEvent* showEvent) override;

private:

    /**
     * Get actions for loaded views
     * @param systemView Whether to include system views or normal views
     * @return Vector of actions
     */
    QVector<QAction*> getLoadedViewsActions(bool systemView);
};
