// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <widgets/FlowLayout.h>

#include <QWidget>
#include <QVBoxLayout>

namespace mv::plugin {
    class Pluginfactory;
}

/**
 * Start page actions widget class
 *
 * Widget class for listing start page actions.
 *
 * @author Thomas Kroes
 */
class StartPageLoadedPluginsWidget : public QWidget
{
protected:

    class PluginWidget : public QWidget
    {
    public:
        PluginWidget(mv::plugin::PluginFactory* pluginfactory, QWidget* parent = nullptr);

    protected:
    private:
    };

public:

    /**
     * Construct with \p parent widget
     * @param parent Pointer to parent widget
     * @param title Title shown in the header
     * @param restyle Whether to change the style to seamlessly integrate with the start page (false for use outside of start page widget)
     */
    StartPageLoadedPluginsWidget(QWidget* parent, const QString& title, bool restyle = true);

    /**
     * Get layout
     * @return Reference to main layout
     */
    QVBoxLayout& getLayout();

private:
    
    /** Update all  custom style elements */
    void updateCustomStyle();

private:
    QVBoxLayout             _layout;                    /** Main layout */
    QScrollArea             _loadedPluginsScrollArea;   /** Scroll area for the loaded plugins widget */
    QWidget                 _loadedPluginsWidget;       /** Widget for the loaded plugins */
    mv::gui::FlowLayout     _loadedPluginsLayout;       /** Flow layout for displaying the loaded plugins */
    bool                    _restyle;                   /** Remember whether the restyle flag was set upon creation */
};
