#include "HelpMenu.h"
#include "PluginManager.h"
#include "CoreInterface.h"

#include <actions/TriggerAction.h>

#include <util/Miscellaneous.h>

using namespace hdps;
using namespace hdps::util;
using namespace hdps::gui;

HelpMenu::HelpMenu(QWidget* parent /*= nullptr*/) :
    QMenu(parent)
{
    setTitle("Help");
    setToolTip("HDPS help");

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
    });
}

