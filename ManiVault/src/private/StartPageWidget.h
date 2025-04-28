// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "PageWidget.h"

#include "StartPageContentWidget.h"

#include <QWidget>

/**
 * Start page widget class
 *
 * Widget class which show various project-related operations.
 *
 * @author Thomas Kroes
 */
class StartPageWidget : public PageWidget
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     */
    StartPageWidget(QWidget* parent = nullptr);

    /**
     * Override show event to update start page actions
     * @param showEvent Pointer to show event which occurred
     */
    void showEvent(QShowEvent* showEvent);

    /**
     * Override to conditionally add the export action
     * @param event Pointer to event that occurred
     * @return
     */
    bool event(QEvent* event) override;

public: // Serialization

    /**
     * Load widget action from variant
     * @param variantMap Variant representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant
     * @return Variant representation of the widget action
     */
    QVariantMap toVariantMap() const override;

private:

    /** Load the current configuration from a file */
    void loadConfiguration();

    /** Save the current configuration to a file */
    void saveConfiguration() const;

public: // Action getters

    mv::gui::TriggerAction& getLoadAction() { return _loadAction; }
    mv::gui::TriggerAction& getSaveAction() { return _saveAction; }

protected:
    StartPageContentWidget          _startPageContentWidget;    /** Widget which contains all the actions */
    mv::gui::TriggerAction          _loadAction;                /** Trigger action for loading a configuration (ony for experimental purposes) */
    mv::gui::TriggerAction          _saveAction;                /** Trigger action for saving a configuration (ony for experimental purposes) */
    mv::gui::HorizontalGroupAction  _fileAction;                /** Group action for load/save configurations */
    mv::gui::VerticalGroupAction    _configurationAction;       /** Group action for the configuration */
};
