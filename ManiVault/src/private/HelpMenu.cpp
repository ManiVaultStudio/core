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
    _aboutProjectAction(nullptr, "About project"),
    _aboutAction(nullptr, "About"),
    _aboutQtAction(nullptr, "About Qt"),
    _aboutThirdPartiesAction(nullptr, "About third-parties")
{
    setTitle("Help");
    setToolTip("ManiVault help");

    _aboutThirdPartiesAction.setMenuRole(QAction::NoRole);
    _aboutQtAction.setMenuRole(QAction::NoRole);
    _aboutProjectAction.setIcon(Application::getIconFont("FontAwesome").getIcon("info"));

    connect(&_aboutProjectAction, &TriggerAction::triggered, this, []() -> void {
        if (!projects().hasProject())
            return;

        projects().getCurrentProject()->getSplashScreenAction().getOpenAction().trigger();
    });

    const auto updateAboutProjectActionReadOnly = [this]() -> void {
        _aboutProjectAction.setVisible(projects().hasProject());
    };

    updateAboutProjectActionReadOnly();

    connect(&projects(), &AbstractProjectManager::projectOpened, this, updateAboutProjectActionReadOnly);
    connect(&projects(), &AbstractProjectManager::projectDestroyed, this, updateAboutProjectActionReadOnly);

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
        if (pluginFactory->hasHelp() && pluginFactory->getNumberOfInstances() >= 1)
            actions << &pluginFactory->getPluginMetadata().getTriggerHelpAction();

    sortActions(actions);

    if (!actions.isEmpty()) {
        auto pluginHelpMenu = new QMenu("Plugin");

        pluginHelpMenu->setToolTip("ManiVault plugin documentation");
        pluginHelpMenu->setIcon(Application::getIconFont("FontAwesome").getIcon("plug"));

        for (auto action : actions)
            pluginHelpMenu->addAction(action);

        addMenu(pluginHelpMenu);
    }

    addSeparator();

    addMenu(mv::help().getVideosMenu());
    addMenu(mv::help().getTutorialsMenu());

    addSeparator();
    addAction(&_aboutProjectAction);

    if(!isEmpty())
        addSeparator();

    addAction(&_aboutAction);
    addAction(&_aboutThirdPartiesAction);
    addAction(&_aboutQtAction);

    connect(&_devDocAction, &QAction::triggered, this, [this](bool) {
        QDesktopServices::openUrl(QUrl("https://github.com/ManiVaultStudio/PublicWiki", QUrl::TolerantMode));
    });

    connect(&_aboutAction, &TriggerAction::triggered, this, &HelpMenu::about);
    connect(&_aboutThirdPartiesAction, &TriggerAction::triggered, this, &HelpMenu::aboutThirdParties);

    connect(&_aboutQtAction, &TriggerAction::triggered, this, [this](bool) {
        QMessageBox::aboutQt(this->parentWidget(), "About Qt");
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
        "&bull; Qt ((L)GPL): <a href=\"https://%4/\">%4</a> "
    ).arg(QStringLiteral("github.com/githubuser0xFFFF/Qt-Advanced-Docking-System"),
          QStringLiteral("github.com/stachenov/quazip"),
          QStringLiteral("zlib.net"),
          QStringLiteral("qt.io"));

    auto msgBox = new QMessageBox(this->parentWidget());

    msgBox->setAttribute(Qt::WA_DeleteOnClose);
    msgBox->setWindowTitle(tr("About Third Parties"));
    msgBox->setText(message);
    msgBox->setIconPixmap(QApplication::windowIcon().pixmap(QSize(64, 64)));
    msgBox->setAttribute(Qt::WA_DeleteOnClose);

    msgBox->open();
}
