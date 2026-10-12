// Author: T. Kroes

#include "HeadlessCoreTestPlugin.h"

#include <CoreInterface.h>

#include <QVariantMap>

#include <utility>

HeadlessCoreTestPlugin::HeadlessCoreTestPlugin(const mv::plugin::PluginFactory* factory) :
    mv::plugin::Plugin(factory)
{
}

void HeadlessCoreTestPlugin::init()
{
    /**
     * Scenario: core.initialized
     *
     * Confirms that the public Core facade is available and that Core has
     * completed initialization before other scenarios are executed.
     */
    registerScenario(
        "core.initialized",
        "Core initialization",
        "Verifies that the ManiVault Core and its managers are initialized.",
        [](QString& error) {
            if (mv::core() == nullptr) {
                error = "The Core facade is unavailable";
                return false;
            }

            if (!mv::core()->isInitialized()) {
                error = "The Core is not initialized";
                return false;
            }

            return true;
        });
}

QVariantList HeadlessCoreTestPlugin::getScenarios() const
{
    QVariantList scenarios;

    for (const auto& scenario : _scenarios) {
        scenarios.push_back(QVariantMap{
            { "id", scenario.id },
            { "title", scenario.title },
            { "description", scenario.description }
        });
    }

    return scenarios;
}

bool HeadlessCoreTestPlugin::runScenario(const QString& scenarioId)
{
    _lastError.clear();

    for (const auto& scenario : _scenarios) {
        if (scenario.id == scenarioId)
            return scenario.runner(_lastError);
    }

    _lastError = QString("Unknown Core test scenario: %1").arg(scenarioId);
    return false;
}

void HeadlessCoreTestPlugin::registerScenario(QString id, QString title, QString description, std::function<bool(QString&)> runner)
{
    _scenarios.push_back({ std::move(id), std::move(title), std::move(description), std::move(runner) });
}

void HeadlessCoreTestPlugin::openProject(const QString& filePath)
{
    mv::projects().openProject(filePath, {}, false, false);
}

HeadlessCoreTestPluginFactory::HeadlessCoreTestPluginFactory() :
    mv::plugin::TestPluginFactory("Headless Core Test")
{
    setAllowPluginCreationFromStandardGui(false);
}

mv::plugin::Plugin* HeadlessCoreTestPluginFactory::produce()
{
    return new HeadlessCoreTestPlugin(this);
}
