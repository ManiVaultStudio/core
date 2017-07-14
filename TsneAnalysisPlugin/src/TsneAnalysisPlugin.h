#ifndef TSNE_ANALYSIS_PLUGIN_H
#define TSNE_ANALYSIS_PLUGIN_H

#include <AnalysisPlugin.h>

#include "TsneAnalysis.h"
#include <widgets/SettingsWidget.h>

#include <QObject>
#include <QComboBox>
#include <QPushButton>

using namespace hdps::plugin;

// =============================================================================
// View
// =============================================================================

class TsneAnalysisPlugin : public QObject, public AnalysisPlugin
{
    Q_OBJECT   
public:
    TsneAnalysisPlugin() : AnalysisPlugin("tSNE Analysis") { }
    ~TsneAnalysisPlugin(void);
    
    void init();

    void dataAdded(const QString name) Q_DECL_OVERRIDE;
    void dataChanged(const QString name) Q_DECL_OVERRIDE;
    void dataRemoved(const QString name) Q_DECL_OVERRIDE;
    void selectionChanged(const QString dataName) Q_DECL_OVERRIDE;
    QStringList supportedDataKinds() Q_DECL_OVERRIDE;
    void onEmbeddingUpdate() const;
public slots:
    void dataSetPicked(const QString& name);
    void startComputation();
private:
    std::unique_ptr<MCV_TsneAnalysis> tsne;
    QString _embedSetName;
};

class TsneSettingsWidget : public SettingsWidget
{
    Q_OBJECT

public:
    TsneSettingsWidget(const TsneAnalysisPlugin* const analysis) {
        startButton.setText("Start tSNE");
        connect(&dataOptions, SIGNAL(currentIndexChanged(QString)), analysis, SLOT(dataSetPicked(QString)));
        connect(&startButton, SIGNAL(clicked()), analysis, SLOT(startComputation()));
    }
public:
    QPushButton startButton;
    QComboBox dataOptions;
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
    ~TsneAnalysisPluginFactory(void) {}
    
    AnalysisPlugin* produce();
};

#endif // TSNE_ANALYSIS_PLUGIN_H
