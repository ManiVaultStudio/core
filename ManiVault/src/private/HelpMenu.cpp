// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "HelpMenu.h"
#include "PluginManager.h"
#include "CoreInterface.h"
#include "UserFeedbackDialog.h"

#include <util/Miscellaneous.h>
#include <actions/TriggerAction.h>

#include <QMessageBox> 
#include <QStringLiteral>
#include <QOperatingSystemVersion>
#include <QDesktopServices>

using namespace mv;
using namespace mv::util;
using namespace mv::gui;
using namespace mv::plugin;

HelpMenu::HelpMenu(QWidget* parent /*= nullptr*/) :
    QMenu(parent),
    _devDocAction(nullptr, "Developer Documentation"),
    _sendFeedbackAction(nullptr, "Send feedback..."),
    _aboutAction(nullptr, QString("About %1").arg(Application::getBaseName())),
    _aboutQtAction(nullptr, "About Qt"),
    _aboutThirdPartiesAction(nullptr, "About third-parties"),
    _releaseNotesAction(nullptr, "Release notes")
{
    setTitle("Help");
    setToolTip("ManiVault help");

    _aboutThirdPartiesAction.setMenuRole(QAction::NoRole);
    _aboutQtAction.setMenuRole(QAction::NoRole);

    _releaseNotesAction.setIconByName("scroll");
    _sendFeedbackAction.setIconByName("comment-dots");

    populate();
}

void HelpMenu::showEvent(QShowEvent* event)
{
    QMenu::showEvent(event);

    if (QOperatingSystemVersion::currentType() == QOperatingSystemVersion::Windows)
        populate();
}

void HelpMenu::populate()
{
    clear();
    
    addAction(&mv::help().getShowLearningCenterPageAction());
    addAction(&_devDocAction);
#ifdef MV_USE_ERROR_LOGGING
    addAction(&_sendFeedbackAction);
#endif
    addSeparator();
    
    QVector<QPointer<TriggerAction>> actions;

    for (auto& pluginFactory : plugins().getPluginFactoriesByTypes({ Type::ANALYSIS, Type::DATA, Type::LOADER, Type::WRITER, Type::TRANSFORMATION, Type::VIEW }))
        if (pluginFactory->hasHelp())
            actions << &pluginFactory->getPluginMetadata().getTriggerHelpAction();

    sortActions(actions);

    if (!actions.isEmpty()) {
        auto pluginHelpMenu = new QMenu("Plugins");

        pluginHelpMenu->setToolTip("ManiVault plugin documentation");
        pluginHelpMenu->setIcon(StyledIcon("plug"));

        for (auto action : actions)
            pluginHelpMenu->addAction(action);

        addMenu(pluginHelpMenu);
    }

    addSeparator();

    addMenu(mv::help().getVideosMenu());
    addMenu(mv::help().getTutorialsMenu());

    if(!isEmpty())
        addSeparator();

    addAction(&_aboutAction);
    addAction(&_aboutThirdPartiesAction);
    addAction(&_aboutQtAction);

    addSeparator();

    addAction(&_releaseNotesAction);

    connect(&_devDocAction, &QAction::triggered, this, [this](bool) {
        QDesktopServices::openUrl(QUrl("https://github.com/ManiVaultStudio/PublicWiki", QUrl::TolerantMode));
    });

#ifdef MV_USE_ERROR_LOGGING
    connect(&_sendFeedbackAction, &TriggerAction::triggered, this, &HelpMenu::sendFeedback, Qt::UniqueConnection);
#endif
    connect(&_aboutAction, &TriggerAction::triggered, this, &HelpMenu::about);
    connect(&_aboutThirdPartiesAction, &TriggerAction::triggered, this, &HelpMenu::aboutThirdParties);

    connect(&_aboutQtAction, &TriggerAction::triggered, this, [this](bool) {
        QMessageBox::aboutQt(this->parentWidget(), "About Qt");
    });

    connect(&_releaseNotesAction, &QAction::triggered, this, [this](bool) {
        const auto applicationVersion = Application::current()->getVersion();

        QDesktopServices::openUrl(QUrl("https://github.com/ManiVaultStudio/core/releases/", QUrl::TolerantMode));
    });
}

void HelpMenu::sendFeedback()
{
    auto feedbackDialog = new UserFeedbackDialog(this->parentWidget());

    feedbackDialog->setAttribute(Qt::WA_DeleteOnClose);
    feedbackDialog->open();
}

void HelpMenu::about() const
{
    QMessageBox::about(this->parentWidget(), tr("About ManiVault"), Application::getAbout());
}

void HelpMenu::aboutThirdParties() const
{
    QString message = QMessageBox::tr(
      "<p>ManiVault uses several third party libraries: </p>"
      "&bull; Qt-Advanced-Docking-System (LGPL v2.1): <a href=\"https://%{qads}/\">%{qads}</a> <br>"
      "&bull; Quazip (LGPL v2.1): <a href=\"https://%{quazip}/\">%{quazip}</a> <br>"
      "&bull; zlib (zlib license): <a href=\"https://%{zlib}/\">%{zlib}</a> <br>"
      "&bull; nlohmann json (MIT license): <a href=\"https://%{json}/\">%{json}</a> <br>"
      "&bull; valijson (BSD-2-Clause license): <a href=\"https://%{valijson}/\">%{valijson}</a> <br>"
      "&bull; biovault_bfloat16 (Apache-2.0): <a href=\"https://%{bfloat16}/\">%{bfloat16}</a> <br>"
      "&bull; Zstandard (BSD License): <a href=\"https://%{zstd}/\">%{zstd}</a> <br>"
      "&bull; Taskflow (MIT license): <a href=\"https://%{taskflow}/\">%{taskflow}</a> <br>"
#ifdef MV_USE_ERROR_LOGGING
      "&bull; sentry (MIT license): <a href=\"https://%{sentry}/\">%{sentry}</a> <br>"
#endif
      "&bull; Qt ((L)GPL): <a href=\"https://%{qt}/\">%{qt}</a> ");

    message.replace("%{qads}", "github.com/githubuser0xFFFF/Qt-Advanced-Docking-System");
    message.replace("%{quazip}", "github.com/stachenov/quazip");
    message.replace("%{zlib}", "zlib.net");
    message.replace("%{json}", "json.nlohmann.me");
    message.replace("%{valijson}", "github.com/tristanpenman/valijson");
    message.replace("%{bfloat16}", "github.com/biovault/biovault_bfloat16");
    message.replace("%{zstd}", "github.com/facebook/zstd");
    message.replace("%{taskflow}", "github.com/taskflow/taskflow");
  #ifdef MV_USE_ERROR_LOGGING
    message.replace("%{sentry}", "sentry.io");
  #endif
    message.replace("%{qt}", "qt.io");

    auto msgBox = new QMessageBox(this->parentWidget());

    msgBox->setAttribute(Qt::WA_DeleteOnClose);
    msgBox->setWindowTitle(tr("About Third Parties"));
    msgBox->setText(message);
    msgBox->setIconPixmap(QApplication::windowIcon().pixmap(QSize(64, 64)));
    msgBox->setAttribute(Qt::WA_DeleteOnClose);

    msgBox->open();
}
