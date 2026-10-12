#pragma once

// Author: T. Kroes

#include <Plugin.h>

#include <QVariantList>

#include <functional>

/**
 * Test-only plugin used by headless Core integration tests.
 *
 * This plugin is built only with MV_BUILD_TESTING and is not installed as a
 * normal application plugin. It is intended to be loaded explicitly by a
 * dedicated headless test host after Core initialization.
 *
 * @author T. Kroes
 */
class HeadlessCoreTestPlugin final : public mv::plugin::Plugin
{
    Q_OBJECT

public:
    explicit HeadlessCoreTestPlugin(const mv::plugin::PluginFactory* factory);

    /** Register the scenarios exposed by this test plugin. */
    void init() override;

    /** Return scenario metadata for developer tools and test runners. */
    Q_INVOKABLE QVariantList getScenarios() const;

    /** Run a scenario by id and return whether it passed. */
    Q_INVOKABLE bool runScenario(const QString& scenarioId);

    /** Return the failure message from the last failed scenario. */
    Q_INVOKABLE QString getLastError() const { return _lastError; }

    /** Open a project through the public Core project-manager facade. */
    void openProject(const QString& filePath);

private:
    /**
     * Describes one independently runnable Core test scenario.
     *
     * A scenario is discoverable through the developer menu and command-line
     * interface, and reports failure details through its runner argument.
     */
    struct Scenario
    {
        QString                      id;          /**< Stable identifier used by the menu and CLI. */
        QString                      title;       /**< Human-readable scenario title. */
        QString                      description; /**< Short explanation shown to developers. */
        std::function<bool(QString&)> runner;     /**< Test body; writes a failure message and returns success. */
    };

    void registerScenario(QString id, QString title, QString description, std::function<bool(QString&)> runner);

    QVector<Scenario> _scenarios;
    QString           _lastError;
};

/**
 * Factory for the testing-only Core scenario plugin.
 *
 * @author T. Kroes
 */
class HeadlessCoreTestPluginFactory final : public mv::plugin::TestPluginFactory
{
    Q_OBJECT
    Q_INTERFACES(mv::plugin::PluginFactory)
    Q_PLUGIN_METADATA(IID "studio.manivault.HeadlessCoreTestPlugin" FILE "HeadlessCoreTestPlugin.json")

public:
    HeadlessCoreTestPluginFactory();

    mv::plugin::Plugin* produce() override;
};
