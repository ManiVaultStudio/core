#include "HelpMenu.h"
#include "PluginManager.h"
#include "CoreInterface.h"

#include <actions/TriggerAction.h>

#include <util/Miscellaneous.h>

using namespace hdps;
using namespace hdps::util;
using namespace hdps::gui;

HelpMenu::HelpMenu(QWidget* parent /*= nullptr*/) :
    QMenu(parent),
    _projectAboutAction(this, "Project about...")
{
    setTitle("Help");
    setToolTip("HDPS help");

    connect(&_projectAboutAction, &TriggerAction::triggered, this, [this]() -> void {
        if (!projects().hasProject())
            return;

        auto& splashScreenAction = projects().getCurrentProject()->getSplashScreenAction();

        connect(&splashScreenAction.getFinishedAction(), &TriggerAction::triggered, this, [this, &splashScreenAction]() -> void {
            splashScreenAction.getCloseManuallyAction().restoreState();
        });

        splashScreenAction.getCloseManuallyAction().cacheState();
        splashScreenAction.getCloseManuallyAction().setChecked(true);

        splashScreenAction.getShowSplashScreenAction().trigger();
    });

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

        if (projects().hasProject()) {
            auto& splashScreenAction = currentProject->getSplashScreenAction();
            
            if (splashScreenAction.getEnabledAction().isChecked()) {
                addSeparator();
                addAction(_projectAboutAction.getMenuAction());
            }
        }
    });
}

