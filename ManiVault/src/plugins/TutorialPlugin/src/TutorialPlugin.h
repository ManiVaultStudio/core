// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "TutorialWidget.h"

#include <ViewPlugin.h>

#include <actions/HorizontalGroupAction.h>

#include <models/LearningCenterTutorialsFilterModel.h>

/**
 * Tutorial scope plugin
 *
 * For viewing tutorial content.
 *
 * @author Thomas Kroes
 */
class TutorialPlugin : public mv::plugin::ViewPlugin
{
    Q_OBJECT
    
public:

    /**
     * Construct with pointer to plugin \p factory
     * @param factory Pointer to plugin factory
     */
    TutorialPlugin(const mv::plugin::PluginFactory* factory);

    /** Perform plugin initialization */
    void init() override;

public: // Serialization

    /**
     * Load plugin from variant map
     * @param variantMap Variant map representation of the plugin
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save plugin to variant map
     * @return Variant map representation of the plugin
     */
    QVariantMap toVariantMap() const override;

private:
    TutorialWidget                          _tutorialWidget;                        /** Tutorial widget */
    mv::gui::HorizontalGroupAction          _horizontalGroupAction;                 /** Horizontal group action for settings */
    mv::LearningCenterTutorialsFilterModel  _learningCenterTutorialsFilterModel;    /** For filtering the learning center tutorials model */
    mv::gui::OptionAction                   _tutorialPickerAction;                  /** Action for picking the tutorial */
    //mv::gui::PluginPickerAction         _sourcePluginPickerAction;      /** Action for picking the source plugin */
};

class TutorialPluginFactory : public mv::plugin::ViewPluginFactory
{
    Q_INTERFACES(mv::plugin::ViewPluginFactory mv::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "studio.manivault.TutorialPlugin"
                      FILE  "TutorialPlugin.json")
    
public:

    /**
     * Get plugin icon
     * @param color Icon color for flat (font) icons
     * @return Icon
     */
    QIcon getIcon(const QColor& color = Qt::black) const override;

    /**
     * Get the URL of the GitHub repository
     * @return URL of the GitHub repository
     */
    QUrl getRepositoryUrl() const override;
    
    /**
     * Produces the plugin
     * @return Pointer to the produced plugin
     */
    mv::plugin::ViewPlugin* produce() override;
};
