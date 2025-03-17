// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "TutorialWidget.h"

#include <ViewPlugin.h>

#include <util/LearningCenterTutorial.h>

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

    /**
     * Get currently selected tutorial
     * @return Pointer to tutorial (if one is selected)
     */
    const mv::util::LearningCenterTutorial* getCurrentTutorial() const;

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

public: // Action getters

    mv::gui::HorizontalGroupAction& getHorizontalGroupAction() { return _horizontalGroupAction; }
    mv::gui::OptionAction& getTutorialPickerAction() { return _tutorialPickerAction; }
    mv::gui::TriggerAction& getOpenInBrowserAction() { return _openInBrowserAction; }

private:
    TutorialWidget                          _tutorialWidget;            /** Tutorial widget */
    mv::gui::HorizontalGroupAction          _horizontalGroupAction;     /** Horizontal group action for settings */
    mv::LearningCenterTutorialsFilterModel  _tutorialsFilterModel;      /** For filtering the learning center tutorials model */
    mv::gui::OptionAction                   _tutorialPickerAction;      /** Action for picking the tutorial */
    mv::gui::TriggerAction                  _openInBrowserAction;       /** Action for opening the tutorial on the ManiVault Studio website */
    bool                                    _autoOpenProject;           /** Boolean determining whether to auto-open the associated tutorial project */
};

class TutorialPluginFactory : public mv::plugin::ViewPluginFactory
{
    Q_INTERFACES(mv::plugin::ViewPluginFactory mv::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "studio.manivault.TutorialPlugin"
                      FILE  "TutorialPlugin.json")

public:

    /** Default constructor */
    TutorialPluginFactory();

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
