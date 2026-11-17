// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QDialog>
#include <QVector>

#include <actions/GroupsAction.h>
#include <actions/GroupAction.h>
#include <actions/OptionsAction.h>
#include <actions/VerticalGroupAction.h>

namespace mv::gui {

class InfoOverlayWidget;
class GlobalSettingsGroupAction;

/**
 * @brief Dialog for viewing and changing application and plugin settings.
 *
 * System and plugin filters control their respective sections. The Advanced filter
 * reveals groups marked with GlobalSettingsGroupAction::setAdvanced(bool). Filter choices
 * are stored in application settings, while the referenced global settings groups are
 * owned by the settings and plugin systems rather than by this dialog.
 *
 * @author Thomas Kroes
 */
class SettingsManagerDialog : public QDialog
{
public:

    /**
     * @brief Constructs the settings manager dialog.
     * @param parent Pointer to parent widget
     */
    SettingsManagerDialog(QWidget* parent = nullptr);

    /**
     * @brief Gets the preferred dialog size.
     * @return Preferred size of the dialog
     */
    QSize sizeHint() const override;

    /**
     * @brief Gets the minimum size hint.
     * @return Same size as sizeHint()
     */
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

private:

    /** Applies the selected System, Plugins, and Advanced filters to settings sections. */
    void updateSettingsFilter();

    /** Updates the empty-settings overlay to match the active category filter. */
    void updateEmptySettingsOverlay();

    /** @brief Refreshes the dialog styling after a theme change. */
    void updateCustomStyle();

private:
    GroupsAction                        _groupsAction;                         /** Dialog-owned container for the settings sections */
    VerticalGroupAction                 _settingsFilterGroupAction;            /** Dialog-owned collapsed toolbar group for filter controls */
    OptionsAction                       _settingsFilterAction;                 /** Dialog-owned, persisted System, Plugins, and Advanced checkboxes */
    QVector<GlobalSettingsGroupAction*> _systemSettingsGroupActions;           /** Non-owning references to core application settings sections */
    QVector<GlobalSettingsGroupAction*> _pluginSettingsGroupActions;           /** Non-owning references to plugin-provided settings sections */
    InfoOverlayWidget*                  _emptySettingsOverlayWidget = nullptr; /** Overlay parented to the groups tree widget, if available */
};

}
