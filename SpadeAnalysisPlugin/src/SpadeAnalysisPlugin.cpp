#include "SpadeAnalysisPlugin.h"

#include "PointsPlugin.h"
#include "ClustersPlugin.h"

#include <QtCore>
#include <QMessageBox>
#include <QtDebug>

#include <assert.h>
#include <algorithm>
#include <functional>
#include <random>

#include <unordered_map>

#ifdef __USE_GCD__
#include <dispatch/dispatch.h>
#else
#include <omp.h>
#endif

#include "external/boruvka/boruvka.h"

#define NO_FILE 0

Q_PLUGIN_METADATA(IID "nl.tudelft.ClusteringPlugin")

// =============================================================================
// View
// =============================================================================

SpadeAnalysisPlugin::~SpadeAnalysisPlugin(void)
{
    
}

void SpadeAnalysisPlugin::init()
{
    _settings = std::make_unique<SpadeSettingsWidget>(this);
}

void SpadeAnalysisPlugin::dataAdded(const QString name)
{
    _settings->dataOptions.addItem(name);
}

void SpadeAnalysisPlugin::dataChanged(const QString name)
{

}

void SpadeAnalysisPlugin::dataRemoved(const QString name)
{
    
}

void SpadeAnalysisPlugin::selectionChanged(const QString dataName)
{

}


QStringList SpadeAnalysisPlugin::supportedDataKinds()
{
    QStringList supportedKinds;
    supportedKinds << "Points";
    return supportedKinds;
}

SettingsWidget* const SpadeAnalysisPlugin::getSettings()
{
    return _settings.get();
}

void SpadeAnalysisPlugin::dataSetPicked(const QString& name)
{

}

void SpadeAnalysisPlugin::startComputation()
{
    QString setName = _settings->dataOptions.currentText();

    // Do nothing if we have no data set selected
    if (setName.isEmpty()) {
        return;
    }

    const hdps::Set* set = _core->requestData(setName);
    const DataTypePlugin* dataPlugin = _core->requestPlugin(set->getDataName());
    const PointsPlugin* points = dynamic_cast<const PointsPlugin*>(dataPlugin);

    _selectedMarkers.push_back(0);
    _selectedMarkers.push_back(1);

    
    //MCV_CytometryData* cytoData = MCV_CytometryData::Instance();

    //if (!cytoData) return;

    _baseIsDirty = _baseIsDirty || _markersDirty;

    bool somethingChanged = false;

    int numFiles = NO_FILE + 1;
    if (_baseIsDirty)
    {
        // reset result memory
        _medianDistance.resize(numFiles, 0.0f);
        _scaledMedianDistance.resize(numFiles, 0.0f);

        _localDensity.resize(numFiles);
        _localDensitySorted.resize(numFiles);

        _selectedSamples.resize(numFiles);
        _selectedSamplesIdxs.resize(numFiles);

        somethingChanged = true;
    }

    for (int f = 0; f < numFiles; f++)
    {

        //if (!loadCachedDensities(f)){
            std::cout << "\nProcessing File: " << f << " (File " << f + 1 << " of " << numFiles << ", containing " << points->data.size() / points->numDimensions << " data points)\n";
            somethingChanged = computeMedianMinimumDistance(*points) || somethingChanged;
            somethingChanged = computeLocalDensities(*points) || somethingChanged;
        //}

        somethingChanged = downsample(*points) || somethingChanged;
    }
    somethingChanged = clusterDownsampledData(*points) || somethingChanged;

    somethingChanged = extractClustersFromDendrogram(*points) || somethingChanged;
    somethingChanged = computeMinimumSpanningTree() || somethingChanged;

    for (int f = 0; f < numFiles; f++)
    {
        somethingChanged = upsampleData(*points) || somethingChanged;
    }
    std::cout << "\n";

    //computeJSONObject();

    if (somethingChanged) {
        //createDerivedData(true);
    }
    else  {
        std::cout << "No parameters changed, no update necessary.\n";
    }

    _markersDirty = false;
    _baseIsDirty = false;
    _downsampledDataIsDirty = false;
    _spanningTreeIsDirty = false;

    // Clustering
    std::unordered_map<int, IndexSet*> clusters;

    unsigned int numPoints = points->data.size() / points->numDimensions;
    for (int i = 0; i < _clusters.size(); i++)
    {
        clusters[i] = (IndexSet*)points->createSet();
        for (int j = 0; j < _clusters[i].size(); j++)
        {
            int idx = _clusters[i][j].second;
            clusters[i]->indices.push_back(idx);
        }
    }

    QString clusterSetName = _core->addData("Clusters", "ClusterSet");
    const ClusterSet* clusterSet = dynamic_cast<ClusterSet*>(_core->requestData(clusterSetName));
    ClustersPlugin* plugin = dynamic_cast<ClustersPlugin*>(_core->requestPlugin(clusterSet->getDataName()));

    for (auto& it : clusters)
    {
        IndexSet* cluster = it.second;
        plugin->addCluster(cluster);
    }

    _core->notifyDataAdded(clusterSetName);
}

bool SpadeAnalysisPlugin::upsampleData(const PointsPlugin& points)
{
    if (!_baseIsDirty && !_downsampledDataIsDirty && !_spanningTreeIsDirty) return false;

    if (NO_FILE == 0){ std::cout << "\nUpsampling data .."; }

    std::cout << "\n	File " << "FILENAME" << " (" << NO_FILE + 1 << " of " << NO_FILE+1 << ").\n	";

    int numDimensions = points.numDimensions;
    int numSamples = points.data.size() / numDimensions;

    for (int i = 0; i < numSamples; i++)
    {
        std::cout << "Upsampling: " << i << std::endl;
        if (i % (numSamples / 10) == 0) std::cout << i / (numSamples / 10) * 10 << "%..";

        // this is already contained in the clustering        
        if (_selectedSamplesIdxs[NO_FILE][i]) continue;

        int numActiveVariables = static_cast<int>(_selectedMarkers.size());
        std::vector<float> currentExpression = std::vector<float>(numActiveVariables);

        for (int j = 0; j < numActiveVariables; j++)
        {
            int markerIdx = _selectedMarkers[j];
            currentExpression[j] = points.data[numDimensions*i + markerIdx];
        }

        //int idx = 0;
        float minDistance = 99999999.9f;
        int closestCluster = -1;
        for (int s = 0; s < _medianClusterExpressions.size(); s++)
        {
            float dist = distance(&currentExpression, &(_medianClusterExpressions[s]));
            if (dist < minDistance){
                minDistance = dist;
                closestCluster = s;
            }
        }
        _clusters[closestCluster].push_back(std::make_pair(NO_FILE, i));
    }
    //computeMedianClusterExpression();

    std::cout << "\n==============================\nClusters\n==============================\n";
    for (int i = 0; i < _clusters.size(); i++)
    {
        std::cout << "[" << i << "] < ";
        
        std::vector<std::pair<int, int>> c = _clusters[i];
        for (auto p: c) {
            //std::cout << ' ' << *it;
            //std::cout << "(" << (*iter)->index << "," << (*iter)->min_dist_to_set << ") ";
            std::cout << p.second << ", ";
        }
        std::cout << ">\n\n";
    }

    std::cout << " done.\n";

    return true;
}

//void SpadeAnalysisPlugin::createDerivedData(bool overwrite)
//{
//    MCV_CytometryData* data = MCV_CytometryData::Instance();
//
//    MCV_DerivedDataClusters* finalClusters = data->derivedDataClusters(_name, overwrite);
//    _name = finalClusters->name();
//
//    assert(finalClusters);
//
//    //std::cout << "Updating SPADE.\n";
//    finalClusters->setClusters(_clusters, _dataSelection, _edges);
//}

// For a random sample of cells computes distance to other cells in high-dim space,
// calculates the minimum of these distances and returns the median of these minima.
bool SpadeAnalysisPlugin::computeMedianMinimumDistance(const PointsPlugin& points)
{
    if (!_baseIsDirty) return false;

    std::cout << "	Computing median minimum distance ..\n";

    int numDimensions = points.numDimensions;
    int numSamples = points.data.size() / numDimensions;

    // How many samples to take for our median calculation, either the value of the parameter, of the number of points if its lower
    int randomSampleSize = std::min(_maxRandomSampleSize, numSamples);

    // select random samples
    qDebug() << "	Creating " << randomSampleSize << " random samples\n		";

    // Create a random sample of indices from our points
    std::vector<int> selectedSampleIdxs = std::vector<int>(randomSampleSize);

    //std::vector<bool> selectedSamples = std::vector<bool>(numSamples, initFlag);

    std::default_random_engine generator;
    std::chrono::high_resolution_clock::duration dur = std::chrono::high_resolution_clock::now() - _timeCreated;
    unsigned int tSeed = (unsigned int)(dur.count());
    generator.seed(tSeed);

    std::uniform_int_distribution<int> distribution(0, numSamples - 1);
    auto rng = std::bind(distribution, generator);

    int j = 0;
    for (int i = 0; i < randomSampleSize; i++)
    {
        int randomIdx = rng();

        selectedSampleIdxs[j++] = randomIdx;

        if (i % (randomSampleSize / 10) == 0) std::cout << i / (randomSampleSize / 10) * 10 << "%..";
    }
    qDebug() << " done.\n";

    // Compute median nearest neighbor distance
    qDebug() << "	Computing median distance to nearest neighbor ";


    std::vector<float> nearestNeighborDistances;
    nearestNeighborDistances.resize(randomSampleSize);

    int threadTracker = 0;
#pragma omp parallel for num_threads(omp_get_max_threads())
    for (int j = 0; j < randomSampleSize; j++) {
        if (j == 0) std::cout << "using " << omp_get_num_threads() << " threads ..\n		";


        if (++threadTracker % (randomSampleSize / 10) == 0) std::cout << (int)(((float)threadTracker / randomSampleSize) * 100) << "%..";

        int i = selectedSampleIdxs[j];

        // Compute minimum distance to other samples
        float minDistance = 99999999.9f;
        for (int s = 0; s < numSamples; s++)
        {
            // do not compare to myself
            if (s == i) continue;
            float dist = distance(&points.data[numDimensions*i], &points.data[numDimensions*s], &_selectedMarkers);
            
            if (dist < minDistance) minDistance = dist;
        }
        nearestNeighborDistances[j] = minDistance;
    }

    // Sort distances
    std::sort(nearestNeighborDistances.begin(), nearestNeighborDistances.end());

    // Take the median distance
    _medianDistance[NO_FILE] = nearestNeighborDistances[randomSampleSize / 2];
    // Scale the median distance by some factor
    _scaledMedianDistance[NO_FILE] = _medianDistance[NO_FILE] * _alpha;

    qDebug() << "done.\n";
    qDebug() << "		Median distance is " << _medianDistance[NO_FILE] << ". ";
    qDebug() << "Scaled: " << _scaledMedianDistance[NO_FILE] << ".\n";

    return true;
}

// Compute the densities of each cell based on how many of their neighbours are closer than the median distance
bool SpadeAnalysisPlugin::computeLocalDensities(const PointsPlugin& points)
{
    if (!_baseIsDirty) return false;

    qDebug() << "	Computing local densities ";

    int numDimensions = points.numDimensions;
    int numSamples = points.data.size() / numDimensions;

    _localDensity[NO_FILE].resize(numSamples);
    std::fill(_localDensity[NO_FILE].begin(), _localDensity[NO_FILE].end(), 0);


    int threadTracker = 0;
#pragma omp parallel for num_threads(omp_get_max_threads()-1)
    for (int i = 0; i < numSamples; i++) {
        if (i == 0) qDebug() << "using " << omp_get_num_threads() << " threads ..\n		";

        // For every point calculate the distance to every other point and compare it to the median distance
        // If it is closer than increment the density by 1
        // TODO make this not calculate double
        for (int s = 0; s < numSamples; s++)
        {
            if (s == i) continue;

            float dist = distance(&points.data[numDimensions*i], &points.data[numDimensions*s], &_selectedMarkers);
            if (dist <= _scaledMedianDistance[NO_FILE])
            {
                _localDensity[NO_FILE][i]++;
                //std::cout << "(" << s << "," << dist << "), ";
            }
        }
        if (++threadTracker % (numSamples / 10) == 0) qDebug() << (int)(((float)threadTracker / numSamples) * 100) << "%..";
    }

    // Sort the densities so we can take the low section of it in downsample()
    _localDensitySorted[NO_FILE] = _localDensity[NO_FILE];
    std::sort(_localDensitySorted[NO_FILE].begin(), _localDensitySorted[NO_FILE].end());

    qDebug() << " done.\n";

    return true;
}

bool SpadeAnalysisPlugin::downsample(const PointsPlugin& points)
{
    if (!_baseIsDirty && !_downsampledDataIsDirty) return false;

    int numDimensions = points.numDimensions;
    int numSamples = points.data.size() / numDimensions;

    assert(_localDensitySorted[fileIndex].size() == numSamples);

    float percentileToIndex = (numSamples - 1) / 100.0f;

    int outlierDensity = _localDensitySorted[NO_FILE][(int)(percentileToIndex * _outlierDensityPercentile)];
    int targetDensity = std::max(1, _localDensitySorted[NO_FILE][(int)(percentileToIndex * _targetDensityPercentile)]);

    _selectedSamplesIdxs[NO_FILE].resize(numSamples);

    // If we want to use all points, don't downsample
    if (_densityLimit > 99.9)
    {
        for (int i = 0; i < numSamples; i++)
        {
            _selectedSamplesIdxs[NO_FILE][i] = true;
        }
    }
    else
    {
        // Target samples to reach after downsampling
        int numTargetSamples = (int)(numSamples * (0.01f * _densityLimit));

        // Find sampling within user defined limits
        int numSelectedSamples = numTargetSamples * 2;
        float probabilityScale = 1.0f;
        int curveExponent = 0;
        qDebug() << "	Downsampling ..\n";
        while (numSelectedSamples > numTargetSamples * 1.05) // allow 5% difference
        {
            curveExponent++;
            qDebug() << "		outlier density = " << outlierDensity << ", target density = " << targetDensity << ", max cellcount = " << numTargetSamples << ", curve exponent = " << curveExponent << " .. ";

            numSelectedSamples = 0;
#pragma omp parallel for reduction(+ : numSelectedSamples)
            for (int i = 0; i < numSamples; i++)
            {
                int density = _localDensity[NO_FILE][i];

                bool isActive = false;

                if (density <= outlierDensity) // discard
                {
                    isActive = false;
                }
                else if (density <= targetDensity) // keep
                {
                    isActive = true;
                }
                else // use density to compute probability
                {
                    //float probability = (float)targetDensity / density * probabilityScale;
                    float probability = pow((float)targetDensity / density * probabilityScale, curveExponent);
                    float random = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

                    //std::cout << "\nProb = " << probability << " rng = " << random;

                    isActive = random < probability;
                }

                _selectedSamplesIdxs[NO_FILE][i] = isActive;
                if (isActive) numSelectedSamples++;
            }

            if (numTargetSamples == 0) break;

            qDebug() << numSelectedSamples << " samples remain.\n";

            if (curveExponent > 10) break;
        }
    }

    _selectedSamples[NO_FILE].clear();
    for (int i = 0; i < numSamples; i++)
    {
        if (_selectedSamplesIdxs[NO_FILE][i])
        {
            _selectedSamples[NO_FILE].push_back(i);
        }
    }

    qDebug() << "		Done, ";
    qDebug() << _selectedSamples[NO_FILE].size() << " (" << _selectedSamples[NO_FILE].size()*100.0f / numSamples << "%) active Samples remain.\n";

    return true;
}

bool SpadeAnalysisPlugin::clusterDownsampledData(const PointsPlugin& points)
{
    if (!_baseIsDirty && !_downsampledDataIsDirty) return false;

    qDebug() << "\nFiles Combined.\n\nComputing hierachical clustering and Dendrogram ..";

    std::list<cPoint_t*> reducedInput;

    for (int f = 0; f < NO_FILE + 1; f++)
    {
        int numDimensions = points.numDimensions;

        int numActiveSamples = static_cast<int>(_selectedSamples[f].size());
        int numActiveVariables = static_cast<int>(_selectedMarkers.size());

        if (numActiveSamples == 0)
        {
            std::cout << "	No active samples in file ";// << cytoData->header(f)->fileName() << ".";
            continue;
        }

        for (int i = 0; i < numActiveSamples; i++)
        {
            int sIdx = _selectedSamples[f][i];
            const float* vec = &points.data[numDimensions * sIdx];

            cPoint_t* p = new cPoint_t();
            p->originalIndex = sIdx;
            p->fileIndex = f;
            p->expression.resize(numActiveVariables);

            for (int j = 0; j < numActiveVariables; j++)
            {
                int vIdx = _selectedMarkers[j];
                float v = vec[vIdx];

                p->expression[j] = v;
            }

            p->min_dist_to_set = 99999999.9;

            reducedInput.push_back(p);
        }
    }

    qDebug() << "\n	Input reduced, " << reducedInput.size() << " points remain.";

    // print input points
    //printf("Inputs:\n");
    //printf("-------\n");
    //int br = 0;
    //for (std::list<cPoint_t*>::iterator iter = reducedInput.begin();
    //	iter != reducedInput.end();
    //	++iter) {
    //	std::cout << "<" << (*iter)->expression[0];
    //	for (int i = 1; i < (*iter)->expression.size(); i++)
    //	{
    //		std::cout << ", " << std::setprecision(3) << (*iter)->expression[i];
    //	}
    //	std::cout << ">\n";
    //	if (br++ > 100) break;
    //}
    //printf("\n");

    _dendrogram.clear();
    hcluster_points(reducedInput, _dendrogram);

    // print output points
    //printf("Outputs:\n");
    //printf("--------\n");
    //br = 0;
    //for (std::map<int, cPoint_t*>::iterator iter = clusteredOutput.begin();
    //	iter != clusteredOutput.end();
    //	++iter) {

    //	std::cout << "[" << iter->first << "] ";
    //	std::cout << "<" << iter->second->expression[0];
    //	for (int i = 1; i < iter->second->expression.size(); i++)
    //	{
    //		std::cout << ", " << std::setprecision(3) << iter->second->expression[i];
    //	}
    //	std::cout << "> R = " << iter->second->min_dist_to_set << ", parent: " << iter->second->parent_index << ". Children: ";
    //	for (std::list<int>::iterator citer = iter->second->children.begin();
    //		citer != iter->second->children.end();

    //		++citer) {
    //		printf("%d ", *citer);
    //	}
    //	printf("\n");
    //	if (br++ > 100) break;
    //}
    //printf("\n");

    return true;
}

bool SpadeAnalysisPlugin::extractClustersFromDendrogram(const PointsPlugin& points)
{
    if (!_baseIsDirty && !_downsampledDataIsDirty && !_spanningTreeIsDirty) return false;

    qDebug() << "	Targeting " << _targetNumberOfClusters << " clusters from Dendrogram .. ";

    std::vector<std::list<cPoint_t*> > clusterList;
    compute_n_clusters(_targetNumberOfClusters, _dendrogram, clusterList);

    qDebug() << "done with " << clusterList.size() << " clusters.\n";

    int numClusteredPoints = 0;

    // organize clusters
    _clusters.clear();
    _clusters.resize(clusterList.size());

#pragma omp parallel for reduction(+ : numClusteredPoints)
    for (int i = 0; i < clusterList.size(); i++)
    {
        //if (i == 0) std::cout << "using " << omp_get_num_threads() << " threads ";
        //std::cout << "i =  " << i << "with thread" << omp_get_thread_num() << "\n";

        std::list<cPoint_t*> l = clusterList[i];
        for (std::list<cPoint_t*>::iterator iter = l.begin(); iter != l.end(); iter++){

            _clusters[i].push_back(std::make_pair((*iter)->fileIndex, (*iter)->originalIndex));
        }

        numClusteredPoints += (int)(_clusters[i].size());
    }

    qDebug() << "	Total number of points in clusters: " << numClusteredPoints << ".\n";

    computeMedianClusterExpression(points);

    //std::cout << "\n==============================\nClusters\n==============================\n";
    //for (int i = 0; i < clusterList.size(); i++)
    //{
    //	std::cout << "[" << i << "] < ";

    //	std::list<cPoint_t*> l = clusterList[i];
    //	for (std::list<cPoint_t*>::iterator iter = l.begin(); iter != l.end(); iter++){
    //		//std::cout << ' ' << *it;
    //		//std::cout << "(" << (*iter)->index << "," << (*iter)->min_dist_to_set << ") ";
    //		std::cout << (*iter)->index-1 << ", ";
    //	}
    //	std::cout << ">\n\n";
    //}

    return true;
}

bool SpadeAnalysisPlugin::computeMinimumSpanningTree()
{
    if (!_baseIsDirty && !_downsampledDataIsDirty && !_spanningTreeIsDirty) return false;

    assert(_clusters.size() == _medianClusterExpressions.size());

    if (_clusters.size() == 0 || _medianClusterExpressions[0].size() == 0) return true;

    qDebug() << "\nBuilding minimum spanning tree.. ";

    if (_minimumSpanningTree) delete _minimumSpanningTree;
    _minimumSpanningTree = new Boruvka();
    _minimumSpanningTree->initGraph(&_medianClusterExpressions);
    _minimumSpanningTree->boruvkaMst();

    _edges = _minimumSpanningTree->edgeList();

    return true;
}

void SpadeAnalysisPlugin::computeMedianClusterExpression(const PointsPlugin& points)
{
    //std::cout << "\nComputing median cluster expression ..";

    //std::vector<float*> rawData = std::vector<float*>(NO_FILE + 1);
    //for (int i = 0; i < rawData.size(); i++)
    //{
    //    rawData[i] = cytoData->rawData(i);
    //}

    int numDimensions = points.numDimensions;
    int numActiveVariables = static_cast<int>(_selectedMarkers.size());
    _medianClusterExpressions.resize(_clusters.size());

#ifndef __USE_GCD__
#pragma omp parallel for
    for (int i = 0; i < _medianClusterExpressions.size(); i++) {
#else
    dispatch_apply(_medianClusterExpressions.size(), dispatch_get_global_queue(0, 0), ^ (size_t i) {
#endif //__USE_GCD__

        _medianClusterExpressions[i].resize(numActiveVariables);
        std::vector<float> values(_clusters[i].size());

        for (int j = 0; j < numActiveVariables; j++)
        {
            int var = _selectedMarkers[j];

            for (int k = 0; k < values.size(); k++)
            {
                int file = _clusters[i][k].first;
                int idx = _clusters[i][k].second * numDimensions + var;

                values[k] = points.data[idx];
            }
            std::sort(values.begin(), values.end());
            _medianClusterExpressions[i][j] = values[_clusters[i].size() / 2];
        }
    }
#ifdef __USE_GCD__
    );
#endif // __USE_GCD__
    //std::cout << "finished.\n";

    //std::cout << "\n==============================\nMedians\n==============================\n";
    //std::cout << "\nCluster 0 Expressions:\n";
    //for (int i = 0; i < _clusters[10].size(); i++)
    //{
    //	int sam = _selectedSamples[_clusters[10][i]];

    //	std::cout << _clusters[10][i] << "//" << sam << " < ";
    //	for (int j = 0; j < numActiveVariables; j++)
    //	{
    //		int var = _selectedMarkers[j];
    //		int idx = sam * numVariables + var;
    //		std::cout << rawData[idx] << " ";
    //	}
    //	std::cout << ">\n";
    //}
    //std::cout << "\n< ";
    //for (int j = 0; j < _medianClusterExpressions[10].size(); j++)
    //{
    //	std::cout << _medianClusterExpressions[10][j] << " ";
    //}
    //std::cout << ">\n";
}

float SpadeAnalysisPlugin::distance(const float* v1, const float* v2, std::vector<int>* idxs)
{
    int n = static_cast<int>(idxs->size());

    std::vector<float> stlv1 = std::vector<float>(n);
    std::vector<float> stlv2 = std::vector<float>(n);

    for (int i = 0; i < n; i++)
    {
        int idx = (*idxs)[i];
        stlv1[i] = v1[idx];
        stlv2[i] = v2[idx];
    }

    return distance(&stlv1, &stlv2);
}

float SpadeAnalysisPlugin::distance(std::vector<float>* v1, std::vector<float>* v2)
{
    assert(v1->size() == v2->size());

    float distance = 0.0f;
    int n = static_cast<int>(v1->size());
    
    for (int i = 0; i < n; i++)
    {
        float dist = std::abs((*v1)[i] - (*v2)[i]);
        distance += dist;
    }

    return distance;
}

// =============================================================================
// Factory
// =============================================================================

AnalysisPlugin* SpadeAnalysisPluginFactory::produce()
{
    return new SpadeAnalysisPlugin();
}
