// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "HelpMenu.h"
#include "PluginManager.h"
#include "CoreInterface.h"

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

void HelpMenu::about() const
{
    QMessageBox::about(this->parentWidget(), tr("About ManiVault"), Application::getAbout());
}

void HelpMenu::aboutThirdParties() const
{
    const QString message = QMessageBox::tr(
        "<p>ManiVault uses several third party libraries: </p>"
        "&bull; Qt-Advanced-Docking-System (LGPL v2.1): <a href=\"https://%1/\">%1</a> <br>"
        "&bull; Quazip (LGPL v2.1): <a href=\"https://%2/\">%2</a> <br>"
        "&bull; zlib (zlib license): <a href=\"https://%3/\">%3</a> <br>"
        "&bull; nlohmann json (MIT license): <a href=\"https://%4/\">%4</a> <br>"
        "&bull; valijson (BSD-2-Clause license): <a href=\"https://%5/\">%5</a> <br>"
        "&bull; biovault_bfloat16 (Apache-2.0): <a href=\"https://%6/\">%6</a> <br>"
        "&bull; Qt ((L)GPL): <a href=\"https://%7/\">%7</a> "
#ifdef MV_USE_ERROR_LOGGING
        "<br> &bull; sentry (MIT license): <a href=\"https://%8/\">%8</a>"
#endif
    ).arg(QStringLiteral("github.com/githubuser0xFFFF/Qt-Advanced-Docking-System"),
          QStringLiteral("github.com/stachenov/quazip"),
          QStringLiteral("zlib.net"),
          QStringLiteral("json.nlohmann.me"),
          QStringLiteral("github.com/tristanpenman/valijson"),
          QStringLiteral("github.com/biovault/biovault_bfloat16"),
          QStringLiteral("qt.io")
#ifdef MV_USE_ERROR_LOGGING
         ,QStringLiteral("sentry.io")
#endif
    );

    auto msgBox = new QMessageBox(this->parentWidget());

    msgBox->setAttribute(Qt::WA_DeleteOnClose);
    msgBox->setWindowTitle(tr("About Third Parties"));
    msgBox->setText(message);
    msgBox->setIconPixmap(QApplication::windowIcon().pixmap(QSize(64, 64)));
    msgBox->setAttribute(Qt::WA_DeleteOnClose);

    msgBox->open();
}
