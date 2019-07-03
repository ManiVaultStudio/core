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

class PointsPlugin;

class SpadeAnalysisPlugin : public QObject, public AnalysisPlugin
{
    Q_OBJECT   
public:
    SpadeAnalysisPlugin() :
        AnalysisPlugin("SPADE Analysis"),
        _baseIsDirty(true),
        _downsampledDataIsDirty(true),
        _spanningTreeIsDirty(true),
        _minimumSpanningTree(NULL),
        _jsonGraph("") { }

    ~SpadeAnalysisPlugin(void) override;
    
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

private:
    bool computeMedianMinimumDistance(const PointsPlugin& points, int maxRandomSampleSize, float alpha);
    bool computeLocalDensities(const PointsPlugin& points);
    bool downsample(const PointsPlugin& points, float densityLimit, float targetDensityPercentile, float outlierDensityPercentile);
    bool clusterDownsampledData(const PointsPlugin& points);
    bool extractClustersFromDendrogram(const PointsPlugin& points, int targetClusters);
    bool computeMinimumSpanningTree();
    bool upsampleData(const PointsPlugin& points);

    void computeMedianClusterExpression(const PointsPlugin& points);

    float distance(const float* v1, const float* v2, std::vector<int>* idxs);
    float distance(std::vector<float>* v1, std::vector<float>* v2);

    std::vector<bool> _markerStates;
    std::vector<int> _selectedMarkers;

    std::chrono::high_resolution_clock::time_point _timeCreated;

    // Selective Recomputation
    SpadeSettings lastSettings;
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
    // ClusterA, ClusterB, strength of connection
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
    ~SpadeAnalysisPluginFactory(void) override {}
    
    AnalysisPlugin* produce() override;
};

#endif // SPADE_ANALYSIS_PLUGIN_H
