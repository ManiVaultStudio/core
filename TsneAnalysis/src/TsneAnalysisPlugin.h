#pragma once

#include <AnalysisPlugin.h>

#include "TsneAnalysis.h"
#include "TsneSettingsWidget.h"

using namespace hdps::plugin;
using namespace hdps::gui;

// =============================================================================
// View
// =============================================================================

class TsneAnalysisPlugin : public QObject, public AnalysisPlugin
{
    Q_OBJECT   
public:
    TsneAnalysisPlugin() : AnalysisPlugin("tSNE Analysis") { }
    ~TsneAnalysisPlugin(void) override;
    
    void init() override;

    void dataAdded(const QString name) Q_DECL_OVERRIDE;
    void dataChanged(const QString name) Q_DECL_OVERRIDE;
    void dataRemoved(const QString name) Q_DECL_OVERRIDE;
    void selectionChanged(const QString dataName) Q_DECL_OVERRIDE;
    QStringList supportedDataKinds() Q_DECL_OVERRIDE;

    SettingsWidget* const getSettings() override;
public slots:
    void dataSetPicked(const QString& name);
    void startComputation();
    void onNewEmbedding();

private:
    void initializeTsne();
    void stopComputation();

    TsneAnalysis _tsne;
    std::unique_ptr<TsneSettingsWidget> _settings;
    QString _embedSetName;
};

// =============================================================================
// Factory
// =============================================================================

class TsneAnalysisPluginFactory : public AnalysisPluginFactory
{
    Q_INTERFACES(hdps::plugin::AnalysisPluginFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.tudelft.TsneAnalysisPlugin"
                      FILE  "TsneAnalysisPlugin.json")
    
public:
    TsneAnalysisPluginFactory(void) {}
    ~TsneAnalysisPluginFactory(void) override {}
    
    AnalysisPlugin* produce() override;
};
