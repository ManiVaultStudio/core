// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "HelpMenu.h"
#include "PluginManager.h"
#include "CoreInterface.h"

#include <util/Miscellaneous.h>
#include <actions/TriggerAction.h>

#include <QMessageBox> 
#include <QStringLiteral> 

using namespace hdps;
using namespace hdps::util;
using namespace hdps::gui;
using namespace hdps::plugin;

HelpMenu::HelpMenu(QWidget* parent /*= nullptr*/) :
    QMenu(parent),
    _aboutAction(nullptr),
    _aboutQt(nullptr),
    _aboutThirdParties(nullptr)
{
    setTitle("Help");
    setToolTip("ManiVault help");

    connect(this, &QMenu::aboutToShow, this, [this]() -> void {
        clear();
        
        QVector<QPointer<TriggerAction>> actions;

        for (auto pluginFactory : plugins().getPluginFactoriesByTypes({ Type::ANALYSIS, Type::DATA, Type::LOADER, Type::WRITER, Type::TRANSFORMATION, Type::VIEW }))
            if (pluginFactory->hasHelp() && pluginFactory->getNumberOfInstances() >= 1)
                actions << &pluginFactory->getTriggerHelpAction();

        sortActions(actions);

        if (!actions.isEmpty()) {
            auto pluginHelpMenu = new QMenu("Plugin");

            pluginHelpMenu->setToolTip("HDPS plugin documentation");
            pluginHelpMenu->setIcon(Application::getIconFont("FontAwesome").getIcon("plug"));

            for (auto action : actions)
                pluginHelpMenu->addAction(action);

            addMenu(pluginHelpMenu);
        }

        auto currentProject = projects().getCurrentProject();

        if (currentProject && currentProject->getSplashScreenAction().getEnabledAction().isChecked()) {
            addSeparator();

            addAction(&currentProject->getSplashScreenAction().getShowSplashScreenAction());
        }

        if(!isEmpty())
            addSeparator();

        // the above clear() deletes all actions whose parent is this
        _aboutAction = new TriggerAction(this, "About");
        _aboutThirdParties = new TriggerAction(this, "About Third Parties");
        _aboutQt = new TriggerAction(this, "About Qt");

        connect(_aboutAction, &hdps::gui::TriggerAction::triggered, this, &HelpMenu::about);
        connect(_aboutThirdParties, &hdps::gui::TriggerAction::triggered, this, &HelpMenu::aboutThirdParties);
        connect(_aboutQt, &hdps::gui::TriggerAction::triggered, this, [this](bool) { QMessageBox::aboutQt(this->parentWidget(), "About Qt"); });

        addAction(_aboutAction);
        addAction(_aboutThirdParties);
        addAction(_aboutQt);
    });
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
