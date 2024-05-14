// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "HelpManager.h"

#include <Application.h>

#include <QDesktopServices>

using namespace mv::gui;
using namespace mv::util;

#ifdef _DEBUG
    //#define HELP_MANAGER_VERBOSE
#endif

namespace mv
{

HelpManager::HelpManager() :
    AbstractHelpManager(),
    _showLearningCenterAction(this, "Learning center"),
    _toDiscordAction(this, "Join us..."),
    _toWebsiteAction(this, "Website"),
    _toWikiAction(this, "Wiki"),
    _toRepositoryAction(this, "Repository")
{
    _showLearningCenterAction.setIconByName("chalkboard-teacher");
    _showLearningCenterAction.setToolTip("Go to the learning center");

    _toWebsiteAction.setIconByName("globe");
    _toWebsiteAction.setDefaultWidgetFlags(TriggerAction::Icon);

    _toWikiAction.setIconByName("book");
    _toWikiAction.setDefaultWidgetFlags(TriggerAction::Icon);

    _toRepositoryAction.setIconByName("code-branch");
    _toRepositoryAction.setDefaultWidgetFlags(TriggerAction::Icon);

    _toDiscordAction.setToolTip("Get in touch on our Discord");
    _toWebsiteAction.setToolTip("Visit our website");
    _toWikiAction.setToolTip("Visit our wiki");
    _toRepositoryAction.setToolTip("Contribute to ManiVault on Github");

    connect(&_toDiscordAction, &TriggerAction::triggered, this, []() -> void {
        QDesktopServices::openUrl(QUrl("https://discord.gg/pVxmC2cSzA"));
    });

    connect(&_toWebsiteAction, &TriggerAction::triggered, this, []() -> void {
        QDesktopServices::openUrl(QUrl("https://www.manivault.studio/"));
    });

    connect(&_toWikiAction, &TriggerAction::triggered, this, []() -> void {
        QDesktopServices::openUrl(QUrl("https://github.com/ManiVaultStudio/PublicWiki/wiki/"));
    });

    connect(&_toRepositoryAction, &TriggerAction::triggered, this, []() -> void {
        QDesktopServices::openUrl(QUrl("https://github.com/ManiVaultStudio"));
    });
}

void HelpManager::initialize()
{
#ifdef HELP_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    AbstractHelpManager::initialize();

    if (isInitialized())
        return;

    beginInitialization();
    endInitialization();
}

void HelpManager::reset()
{
}

}
