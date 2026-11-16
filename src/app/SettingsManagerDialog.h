// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QDialog>
#include <QVector>

#include <actions/GroupsAction.h>
#include <actions/GroupAction.h>
#include <actions/OptionsAction.h>

namespace mv::gui {

class InfoOverlayWidget;

/**
 * Settings manager dialog class
 * 
 * Dialog class for modifying settings.
 *
 * @author Thomas Kroes
 */
class SettingsManagerDialog : public QDialog
{
public:

    /**
     * Construct a dialog for the settings manager with \p with parent widget
     * @param parent Pointer to parent widget
     */
    SettingsManagerDialog(QWidget* parent = nullptr);

    /** Get preferred size */
    QSize sizeHint() const override;

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

private:

    /** Applies the selected System and Plugins tags to the settings sections. */
    void updateSettingsFilter();

    /** Updates the empty-settings overlay to match the active category filter. */
    void updateEmptySettingsOverlay();

    /** Update styling after a theme change */
    void updateCustomStyle();

private:
    GroupsAction                _groupsAction;               /** Groups action for all global settings sections */
    OptionsAction               _settingsFilterAction;       /** Tagged filter for system and plugin settings */
    QVector<GroupAction*>       _systemSettingsGroupActions; /** Core application settings sections */
    QVector<GroupAction*>       _pluginSettingsGroupActions; /** Plugin-provided settings sections */
    InfoOverlayWidget*          _emptySettingsOverlayWidget = nullptr; /** Overlay shown when no editable settings are available */
};

}
