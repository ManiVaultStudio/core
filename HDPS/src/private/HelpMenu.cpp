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
#include <QDesktopServices>

using namespace hdps;
using namespace hdps::util;
using namespace hdps::gui;
using namespace hdps::plugin;

HelpMenu::HelpMenu(QWidget* parent /*= nullptr*/) :
    QMenu(parent),
    _devDocAction((nullptr)),
    _aboutAction(nullptr),
    _aboutQt(nullptr),
    _aboutThirdParties(nullptr)
{
    setTitle("Help");

    
    
}

void HelpMenu::initialize()
{
    auto currentProject = projects().getCurrentProject();
    bool readOnlyProject = currentProject && currentProject->getReadOnlyAction().isChecked();

    if (readOnlyProject)
    {
        setToolTip(currentProject->getTitleAction().getString() + " help");
    }
    else
    {
        setToolTip("ManiVault help");

        // initialize static actions
        _devDocAction = new TriggerAction(this, "Developer Documentation");
        connect(_devDocAction, &QAction::triggered, this, [this](bool) { QDesktopServices::openUrl(QUrl("https://github.com/ManiVaultStudio/PublicWiki", QUrl::TolerantMode)); });

        _aboutAction = new TriggerAction(this, "About ManiVault");
        connect(_aboutAction, &hdps::gui::TriggerAction::triggered, this, &HelpMenu::about);

        _aboutQt = new TriggerAction(this, "About Qt");
        _aboutQt->setMenuRole(QAction::NoRole);
        connect(_aboutQt, &hdps::gui::TriggerAction::triggered, this, [this](bool) { QMessageBox::aboutQt(this->parentWidget(), "About Qt"); });


    }

    _aboutThirdParties = new TriggerAction(this, "About Third Parties...");
    _aboutThirdParties->setMenuRole(QAction::NoRole);
    connect(_aboutThirdParties, &hdps::gui::TriggerAction::triggered, this, &HelpMenu::aboutThirdParties);

    // macOS does not like populating the menu on show, so we rather do it explicitly here
    populate();
}


void HelpMenu::populate()
{

    auto currentProject = projects().getCurrentProject();
    bool readOnlyProject = currentProject && currentProject->getReadOnlyAction().isChecked();

    if (_devDocAction)
    {
        addAction(_devDocAction);
        addSeparator();
    }



    QVector<QPointer<TriggerAction>> actions;

    for (auto& pluginFactory : plugins().getPluginFactoriesByTypes({ Type::ANALYSIS, Type::DATA, Type::LOADER, Type::WRITER, Type::TRANSFORMATION, Type::VIEW }))
        if (pluginFactory->hasHelp() && pluginFactory->getNumberOfInstances() >= 1)
            actions << &pluginFactory->getTriggerHelpAction();

    sortActions(actions);

    if (!readOnlyProject)
    {
        if (!actions.isEmpty()) {
            auto pluginHelpMenu = new QMenu("Plugin");

            pluginHelpMenu->setToolTip("ManiVault plugin documentation");
            pluginHelpMenu->setIcon(Application::getIconFont("FontAwesome").getIcon("plug"));

            for (auto action : actions)
                pluginHelpMenu->addAction(action);

            addMenu(pluginHelpMenu);
        }
    }

    if (currentProject && currentProject->getSplashScreenAction().getEnabledAction().isChecked()) {
        addSeparator();

        addAction(&currentProject->getSplashScreenAction().getShowSplashScreenAction());
    }

    if(!isEmpty())
        addSeparator();

    if(_aboutAction)
    {
        addAction(_aboutAction);
    }

    if(_aboutThirdParties)
    {
        addAction(_aboutThirdParties);
    }
    
    if (_aboutQt)
    {
        addAction(_aboutQt);
    }
}

void HelpMenu::about()
{
    QMessageBox::about(this->parentWidget(), tr("About ManiVault"),
        tr("<p>ManiVault is a flexible and extensible visual analytics framework for high-dimensional data.<br>"
            "For more information, please visit: <a href=\"http://%2/\">%2</a> </p>"
            "This software is licensed under the GNU Lesser General Public License v3.0.<br>"
            "Copyright (C) %1 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)"
        ).arg(QStringLiteral("2023"),
              QStringLiteral("github.com/ManiVaultStudio"))
    );
}

void HelpMenu::aboutThirdParties()
{
    const QString message = QMessageBox::tr(
        "<p>ManiVault uses several third party libraries: </p>"
        "&bull; Qt-Advanced-Docking-System (LGPL v2.1): <a href=\"http://%1/\">%1</a> <br>"
        "&bull; Quazip (LGPL v2.1): <a href=\"http://%2/\">%2</a> <br>"
        "&bull; Qt ((L)GPL): <a href=\"http://%3/\">%3</a> "
    ).arg(QStringLiteral("github.com/githubuser0xFFFF/Qt-Advanced-Docking-System"),
          QStringLiteral("github.com/stachenov/quazip"),
          QStringLiteral("qt.io"));

    QMessageBox* msgBox = new QMessageBox(this->parentWidget());
    msgBox->setAttribute(Qt::WA_DeleteOnClose);
    msgBox->setWindowTitle(tr("About Third Parties"));
    msgBox->setText(message);
    msgBox->setIconPixmap(QApplication::windowIcon().pixmap(QSize(64, 64)));
    msgBox->setAttribute(Qt::WA_DeleteOnClose);

    msgBox->open();
}
