#ifndef SPADE_ANALYSIS_PLUGIN_H
#define SPADE_ANALYSIS_PLUGIN_H

#include <AnalysisPlugin.h>

#include "SpadeSettingsWidget.h"

#include "external/hcluster/hcluster.h"

#include <chrono>

using namespace hdps::plugin;
using namespace hdps::gui;

// =============================================================================
// View
// =============================================================================

class map;

class Boruvka;

class SpadeAnalysisPlugin : public QObject, public AnalysisPlugin
{
    Q_OBJECT   
public:
    SpadeAnalysisPlugin() :
        AnalysisPlugin("SPADE Analysis"),
        _maxRandomSampleSize(2000),
        _alpha(3.0),
        _targetDensityPercentile(3.0),
        _outlierDensityPercentile(1.0),
        _densityLimit(10.0),
        _targetNumberOfClusters(50),
        _baseIsDirty(true),
        _downsampledDataIsDirty(true),
        _spanningTreeIsDirty(true),
        _minimumSpanningTree(NULL),
        _jsonGraph("") { }

    ~SpadeAnalysisPlugin(void);
    
    void init();

    void dataAdded(const QString name) Q_DECL_OVERRIDE;
    void dataChanged(const QString name) Q_DECL_OVERRIDE;
    void dataRemoved(const QString name) Q_DECL_OVERRIDE;
    void selectionChanged(const QString dataName) Q_DECL_OVERRIDE;
    QStringList supportedDataKinds() Q_DECL_OVERRIDE;

    SettingsWidget* const getSettings();
public slots:
    void dataSetPicked(const QString& name);

    void startComputation();

private:
    bool computeMedianMinimumDistance(int fileIndex);
    bool computeLocalDensities(int fileIndex);
    bool downsample(int fileIndex);
    bool clusterDownsampledData();
    bool extractClustersFromDendrogram();
    bool computeMinimumSpanningTree();
    bool upsampleData(int fileIndex);

    void createDerivedData(bool overwrite);

    void computeMedianClusterExpression();

    float distance(float* v1, float* v2, std::vector<int>* idxs);
    float distance(std::vector<float>* v1, std::vector<float>* v2);

    std::vector<bool> _markerStates;
    std::vector<int> _selectedMarkers;

    // Parameters
    int _maxRandomSampleSize;
    float _alpha;
    float _targetDensityPercentile;
    float _outlierDensityPercentile;
    float _densityLimit;
    int _targetNumberOfClusters;

    std::chrono::high_resolution_clock::time_point _timeCreated;

    // Selective Recomputation
    bool _baseIsDirty;
    bool _downsampledDataIsDirty;
    bool _spanningTreeIsDirty;
    bool _markersDirty;

    // Results
    std::vector<float> _medianDistance;
    std::vector<float> _scaledMedianDistance;

    std::vector< std::vector<int> > _localDensity;
    std::vector< std::vector<int> > _localDensitySorted;

    std::vector< std::vector<int> > _selectedSamples;
    std::vector< std::vector<bool> > _selectedSamplesIdxs;

    std::map<int, cPoint_t*> _dendrogram;

    std::vector< std::vector< std::pair <int, int> > > _clusters;
    std::vector< std::tuple<int, int, float> > _edges;
    std::vector< std::vector<float> > _medianClusterExpressions;

    Boruvka* _minimumSpanningTree;

    std::string _jsonGraph;

    std::unique_ptr<SpadeSettingsWidget> _settings;
};

// =============================================================================
// Factory
// =============================================================================

class SpadeAnalysisPluginFactory : public AnalysisPluginFactory
{
    Q_INTERFACES(hdps::plugin::AnalysisPluginFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.tudelft.SpadeAnalysisPlugin"
                      FILE  "SpadeAnalysisPlugin.json")
    
public:
    SpadeAnalysisPluginFactory(void) {}
    ~SpadeAnalysisPluginFactory(void) {}
    
    AnalysisPlugin* produce();
};

#endif // SPADE_ANALYSIS_PLUGIN_H
