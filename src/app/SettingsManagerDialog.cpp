// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SettingsManagerDialog.h"

#include <CoreInterface.h>
#include <QOperatingSystemVersion>
#include <QVBoxLayout>
#include <QStyle>

#include <algorithm>

#include <widgets/InfoOverlayWidget.h>

using namespace mv;
using namespace mv::util;

#ifdef _DEBUG
    #define SETTINGS_MANAGER_DIALOG_VERBOSE
#endif

namespace mv::gui {

SettingsManagerDialog::SettingsManagerDialog(QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _groupsAction(this, "Groups"),
    _settingsFilterAction(this, "Filter settings", { "System", "Plugins" }, { "System", "Plugins" })
{
    setWindowIcon(StyledIcon("gears"));
    setWindowModality(Qt::ApplicationModal);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setWindowTitle("Settings");
    setMinimumWidth(800);
    setMinimumHeight(560);
    resize(900, 600);

    auto layout = new QVBoxLayout();

    setLayout(layout);

    _settingsFilterAction.setDefaultWidgetFlags(OptionsAction::Tags);
    _settingsFilterAction.setToolTip("Choose whether system settings, plugin settings, or both are shown.");
    layout->addWidget(_settingsFilterAction.createWidget(this, OptionsAction::Tags));

    auto groupsWidget = _groupsAction.createWidget(this);
    layout->addWidget(groupsWidget);

    if (auto treeWidget = groupsWidget->findChild<QTreeWidget*>()) {
        _emptySettingsOverlayWidget = new InfoOverlayWidget(treeWidget);
        _emptySettingsOverlayWidget->getWidgetFader().setOpacity(0.0f);
        _emptySettingsOverlayWidget->getWidgetFader().setMaximumOpacity(0.5f);
        _emptySettingsOverlayWidget->getWidgetFader().setFadeInDuration(100);
        _emptySettingsOverlayWidget->getWidgetFader().setFadeOutDuration(300);
    }

    connect(&_settingsFilterAction, &OptionsAction::selectedOptionsChanged, this, [this] { updateSettingsFilter(); });

#ifdef MV_USE_ERROR_LOGGING
    _groupsAction.addGroupAction(&mv::settings().getErrorLoggingSettingsAction());
    _systemSettingsGroupActions.append(&mv::settings().getErrorLoggingSettingsAction());
#endif

    _groupsAction.addGroupAction(&mv::settings().getAppearanceSettingsAction());
    _systemSettingsGroupActions.append(&mv::settings().getAppearanceSettingsAction());
    _groupsAction.addGroupAction(&mv::settings().getParametersSettings());
    _systemSettingsGroupActions.append(&mv::settings().getParametersSettings());
    _groupsAction.addGroupAction(&mv::settings().getMiscellaneousSettings());
    _systemSettingsGroupActions.append(&mv::settings().getMiscellaneousSettings());
    _groupsAction.addGroupAction(&mv::settings().getTasksSettingsAction());
    _systemSettingsGroupActions.append(&mv::settings().getTasksSettingsAction());
    _groupsAction.addGroupAction(&mv::settings().getTemporaryDirectoriesSettingsAction());
    _systemSettingsGroupActions.append(&mv::settings().getTemporaryDirectoriesSettingsAction());

    for (auto pluginFactory : mv::plugins().getPluginFactoriesByTypes()) {
        if (auto pluginGlobalSettingsGroupAction = pluginFactory->getGlobalSettingsGroupAction())
        {
            _groupsAction.addGroupAction(pluginGlobalSettingsGroupAction);
            _pluginSettingsGroupActions.append(pluginGlobalSettingsGroupAction);
        }
    }

    updateSettingsFilter();
    mv::settings().getTemporaryDirectoriesSettingsAction().refreshTemporaryStorageStatus();

    connect(&mv::theme(), &AbstractThemeManager::colorSchemeChanged, this, &SettingsManagerDialog::updateCustomStyle);
}

void SettingsManagerDialog::updateSettingsFilter()
{
    const auto selectedOptions = _settingsFilterAction.getSelectedOptions();
    const auto showSystemSettings = selectedOptions.contains("System");
    const auto showPluginSettings = selectedOptions.contains("Plugins");

    _groupsAction.setGroupActionVisibilityFilter([this, showSystemSettings, showPluginSettings](GroupAction* groupAction) {
        const auto isSystemSettings = _systemSettingsGroupActions.contains(groupAction);
        const auto isPluginSettings = _pluginSettingsGroupActions.contains(groupAction);

        return (isSystemSettings && showSystemSettings) ||
            (isPluginSettings && showPluginSettings) ||
            (!isSystemSettings && !isPluginSettings);
    });

    updateEmptySettingsOverlay();
}

void SettingsManagerDialog::updateEmptySettingsOverlay()
{
    if (!_emptySettingsOverlayWidget)
        return;

    const auto hasEditableSettings = [this](const QVector<GroupAction*>& groupActions) {
        return std::any_of(groupActions.begin(), groupActions.end(), [](GroupAction* groupAction) {
            return !groupAction->getActions().isEmpty();
        });
    };

    const auto selectedOptions = _settingsFilterAction.getSelectedOptions();
    const auto hasVisibleSettings =
        (selectedOptions.contains("System") && hasEditableSettings(_systemSettingsGroupActions)) ||
        (selectedOptions.contains("Plugins") && hasEditableSettings(_pluginSettingsGroupActions));
    auto& widgetFader = _emptySettingsOverlayWidget->getWidgetFader();

    if (hasVisibleSettings) {
        widgetFader.fadeOut();
    } else {
        _emptySettingsOverlayWidget->set(StyledIcon("gears"), "No editable settings", "Try changing the System / Plugins filter.");
        widgetFader.fadeIn();
    }
}

QSize SettingsManagerDialog::sizeHint() const
{
    return { 800, 600 };
}

void SettingsManagerDialog::updateCustomStyle()
{
    const auto widgets = findChildren<QWidget*>();

    setWindowIcon(StyledIcon("gears"));

    style()->unpolish(this);
    style()->polish(this);
    update();

    for (auto widget : widgets) {
        widget->style()->unpolish(widget);
        widget->style()->polish(widget);
        widget->update();
    }
}

}
