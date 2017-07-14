#include "TsneAnalysis.h"

#include <vector>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <stdio.h>

//#include "MCV_UiInterface.h"
//#include "MCV_CytometryData.h"
//#include "MCV_DerivedData.h"
//#include "MCV_DerivedDataPoints.h"
//#include "MCV_DerivedDataClusters.h"
//#include "MCV_FcsHeader.h"

// TSNE
#include "vptree.h"
#include "gradient_descent.h"
#include "tsne_errors.h"

#include "TsneAnalysisPlugin.h"
#include <QDebug>
#include <QApplication>

#include "timings/scoped_timers.h"

MCV_TsneAnalysis::MCV_TsneAnalysis() :
_verbose(false),
_skipNormalization(false),
_iterations(1000),
_numTrees(4),
_numChecks(1024),
_exaggerationIter(250),
_expDecay(70),
_perplexity(30),
_numDimensionsOutput(2),
_isGradientDescentRunning(false),
_isTsneRunning(false),
_isMarkedForDeletion(false),
_numDataPoints(0),
_continueFromIteration(0),
_radius(0.0f)
//_numDiscreteMetaValues(0)
{
    //_type = MCV_ANALYSIS_TYPE_TSNE;
}

void MCV_TsneAnalysis::init(std::string name)
{
}

void MCV_TsneAnalysis::computeTSNE()
{
    //initTSNE();

    //computeGradientDescent();
}

void MCV_TsneAnalysis::computeGradientDescent(const TsneAnalysisPlugin& plugin)
{
    initGradientDescent();

    embed(plugin);
}

void MCV_TsneAnalysis::initTSNE(const std::vector<float>* data, const int numDimensions)
{
    //_pointReferences = *(_dataSelection->data());

    //if (_pointReferences.size() < 1) return;

    //if (_perplexity * 4 > _pointReferences.size())
    //{
    //    _perplexity = (int)(_pointReferences.size() / 4);
    //}

    //// Test data
    //std::vector<std::vector<float>> testData(5);
    //testData[0] = std::vector<float> { 0.0f, 1.0f, 2.0f, 3.0f };
    //testData[1] = std::vector<float> { 4.0f, 1.0f, 2.0f, 3.0f };
    //testData[2] = std::vector<float> { 4.0f, 1.0f, 3.0f, 3.0f };
    //testData[3] = std::vector<float> { 0.0f, 4.0f, 2.0f, 2.0f };
    //testData[4] = std::vector<float> { 0.0f, 4.0f, 2.0f, 1.0f };
    qDebug() << "Test data created.";
    _numDataPoints = (*data).size() / numDimensions;// testData.size();// (int)(_pointReferences.size());
    // Number of total dimensions
    int numMarkerDimensions = numDimensions;// testData[0].size();// cytoData->combinedHeader()->numVariables();
    // Number of selected dimensions
    //int numDimensions = testData[0].size();// (int)(_selectedMarkers.size());
    qDebug() << "Variables set. Num dims: " << numDimensions << " Num data points: " << _numDataPoints;
    //// Collect raw data from all files in cytoData and add them to the tmpData array of float arrays
    //std::vector<float*> tmpData = std::vector<float*>(cytoData->numFiles());
    //for (int i = 0; i < tmpData.size(); i++)
    //{
    //    tmpData[i] = cytoData->rawData(i);
    //}

    // Input data
    _tSNEData.clear();
    _tSNEData.resize(_numDataPoints * numDimensions);
    qDebug() << "TSNE data allocated.";
    //_fileIdxs.resize(_numDataPoints);
    //_markers.resize(_numDataPoints * numMarkerDimensions);

    // Fill tSNEData with points * dims
    for (int i = 0; i < _numDataPoints; i++)
    {
        //int file = _pointReferences[i].first;
        //_fileIdxs[i] = file;
        //int idx = _pointReferences[i].second;
        //int inOffset = idx * numMarkerDimensions;
        //int outOffset = i * numDimensions;

        for (int j = 0; j < numDimensions; j++)
        {
            //int varIdx = _selectedMarkers[j];

            _tSNEData[i * numDimensions + j] = static_cast<double>((*data)[i * numDimensions + j]);//tmpData[file][inOffset + varIdx]);
        }

        //FIXME: should marker data belong to virtual datasets (?)
        //memcpy(&(_markers[i * numMarkerDimensions]), &(tmpData[file][inOffset]), sizeof(float)* numMarkerDimensions);
    }
    qDebug() << "Data copied.";
    //_numDiscreteMetaValues = _dataSelection->numDiscreteMetaValues();
    
    /// Computation of the high dimensional similarities
    _output.resize(_numDataPoints * _numDimensionsOutput);
    _outputDouble.resize(_numDataPoints * _numDimensionsOutput);
    qDebug() << "Output allocated.";
    qDebug() << "Out before: " << _tSNEData[0];
    {
        TSNEApproxInitializer<> initializer;

        //initializer._log = &_log;
        initializer._verbose = _verbose;
        initializer._param._perplexity = _perplexity;
        initializer._param._perplexity_multiplier = 3;
        initializer._param._num_trees = _numTrees;
        initializer._param._num_checks = _numChecks;
        initializer._param._skip_normalization = _skipNormalization;
        qDebug() << "tSNE initialized.";
        _sparseMatrix.Clear();
        _sparseMatrix.Resize(_numDataPoints);
        qDebug() << "Sparse matrix allocated.";
        double t = 0.0;
        {
            nut::ScopedTimer<double> timer(t);
            initializer.ComputeHighDimensionalProbDistributions(_tSNEData.data(), _numDataPoints, numDimensions, _sparseMatrix);
        }
        qDebug() << "Probability distributions calculated.";
        std::cout << "\n================================================================================\n";
        std::cout << "A-tSNE: Compute probability distribution: " << t / 1000 << " seconds";
        std::cout << "\n--------------------------------------------------------------------------------\n";
    }
    qDebug() << "Out after: " << _tSNEData[0];
    //_uiInterface->tsneInitialized();
}

void MCV_TsneAnalysis::initGradientDescent()
{
    _continueFromIteration = 0;

    _isTsneRunning = true;

    _gradientDescent = atsne::GradientDescent<>();

    _flags.resize(_numDataPoints);
    std::fill(_flags.begin(), _flags.end(), 0);

    //_gradientDescent._log = &_log;
    _gradientDescent._verbose = _verbose;

    _gradientDescent._param._stop_lying_iter = _exaggerationIter;
    _gradientDescent._param._mom_switching_iter = _exaggerationIter;

    if (_expDecay > 1){
        double exaggeration_factor = 10 + _numDataPoints / 5000.;
        double decay_factor = std::exp(std::log(1 / exaggeration_factor) / double(_expDecay));
        //nut::SecureLogValue(&_log, "Decay factor", decay_factor, _verbose);

        _gradientDescent._param._use_exponential_decay_as_early_exaggeration_removal = true;
        _gradientDescent._param._exponential_decay = decay_factor;
        _gradientDescent._param._stop_lying_iter = _exaggerationIter - _expDecay;
    }
    else{
        _gradientDescent._param._use_exponential_decay_as_early_exaggeration_removal = false;
        _gradientDescent._param._stop_lying_iter = _exaggerationIter;
    }

    _gradientDescent._param._theta = std::min(0.5, std::max(0.0, (_numDataPoints - 1000.0)*0.00005));

    _gradientDescent.Initialize(&_sparseMatrix, _flags.data(), _tSNEData.data(), _outputDouble.data(), _numDimensionsOutput);

    copyFloatOutput();
}

// Computing gradient descent
void MCV_TsneAnalysis::embed(const TsneAnalysisPlugin& plugin)
{

    double t = 0.0;
    double kld = 0.0;
    double kldmin = 0.0;
    double kldmax = 0.0;
    {
        
        std::cout << "A-tSNE: Computing gradient descent..\n";
        _isGradientDescentRunning = true;

        // Performs gradient descent for every iteration
        for (int iter = 0; iter < _iterations; ++iter){
            nut::ScopedTimer<double> timer(t);
            if (!_isGradientDescentRunning)
            {
                _continueFromIteration = iter;
                break;
            }

            _gradientDescent.DoAnIteration();
            //nut::SecureLogValue(&_log, "Iter", iter, _verbose);

            _radius = (float)(std::max(_gradientDescent.max_per_dimension()[0] * 1.1, _gradientDescent.max_per_dimension()[1] * 1.1));

            copyFloatOutput();

            plugin.onEmbeddingUpdate();
            qApp->processEvents();

            if ((iter + 1) % 100 == 0){
                TSNEErrorUtils<>::ComputeBarnesHutTSNEErrorWithTreeComputation(_sparseMatrix, _outputDouble.data(), 2, kld, kldmin, kldmax, _gradientDescent._param._theta);
                std::cout << "<Name goes here>" << ": iteration: " << iter + 1 << ", KL-divergence: " << kld << std::endl;
            }

            if (!_isMarkedForDeletion)
            {
                //_uiInterface->analysisUpdated((void*)this);
            }
            qDebug() << "Time: " << t;
        }
        
        _isGradientDescentRunning = false;
        _isTsneRunning = false;
    }
    TSNEErrorUtils<>::ComputeBarnesHutTSNEErrorWithTreeComputation(_sparseMatrix, _outputDouble.data(), 2, kld, kldmin, kldmax, _gradientDescent._param._theta);
    std::cout << "--------------------------------------------------------------------------------\n";
    std::cout << "A-tSNE: Finished embedding of " << "<Name goes here>" << " in: " << t / 1000 << " seconds " << ".KL - divergence is : " << kld;
    std::cout << "\n================================================================================\n";


    //if (_isMarkedForDeletion)
    //{
    //    _uiInterface->readyForDeletion((void*)this);
    //}
    //else
    //{
    //    _uiInterface->analysisFinished((void*)this);
    //}
}

// Copy tSNE output to our output
void MCV_TsneAnalysis::copyFloatOutput()
{
    _output.assign(_outputDouble.begin(), _outputDouble.end());

    //std::cout << "Double: " << _outputDouble[0] << " " << _outputDouble[_outputDouble.size() - 1] << "\n";
    //std::cout << "Float: " << _output[0] << " " << _output[_outputDouble.size() - 1] << "\n\n";
}

void MCV_TsneAnalysis::removePoints()
{
}

int MCV_TsneAnalysis::numDataPoints()
{
    return _numDataPoints;
}

float MCV_TsneAnalysis::radius()
{
    return _radius;
}

std::vector<typename atsne::GradientDescent<>::flag_type>* MCV_TsneAnalysis::flags()
{
    return &_flags;
}

std::vector<float>* MCV_TsneAnalysis::output()
{
    return &_output;
}

//std::vector<int>* MCV_TsneAnalysis::outputFileIdxs()
//{
//    return &_fileIdxs;
//}

//std::vector<float>* MCV_TsneAnalysis::markers()
//{
//    return &_markers;
//}


std::vector<float>* MCV_TsneAnalysis::densityGradientMap()
{
    return &_densityGradientMap;
}

// FIXME: remove this and make users acces data via MCV_CytometryData::Instance()->derivedDataPoints(NAME)
//std::vector< std::pair<int, int> >* MCV_TsneAnalysis::pointReferences()
//{
//    return &_pointReferences;
//}

//int MCV_TsneAnalysis::numDiscreteMetaValues()
//{
//    return _numDiscreteMetaValues;
//}

void MCV_TsneAnalysis::setVerbose(bool verbose)
{
    _verbose = verbose;
}

void MCV_TsneAnalysis::setSkipNormalization(bool skipNormalization)
{
    _skipNormalization = skipNormalization;
}

void MCV_TsneAnalysis::setIterations(int iterations)
{
    _iterations = iterations;
}

void MCV_TsneAnalysis::setNumTrees(int numTrees)
{
    _numTrees = numTrees;
}

void MCV_TsneAnalysis::setNumChecks(int numChecks)
{
    _numChecks = numChecks;
}

void MCV_TsneAnalysis::setExaggerationIter(int exaggerationIter)
{
    _exaggerationIter = exaggerationIter;
}

void MCV_TsneAnalysis::setExpDecay(int expDecay)
{
    _expDecay = expDecay;
}

void MCV_TsneAnalysis::setPerplexity(int perplexity)
{
    _perplexity = perplexity;
}

void MCV_TsneAnalysis::setNumDimensionsOutput(int numDimensionsOutput)
{
    _numDimensionsOutput = numDimensionsOutput;
}

void MCV_TsneAnalysis::stopGradientDescent()
{
    _isGradientDescentRunning = false;
}

void MCV_TsneAnalysis::markForDeletion()
{
    _isMarkedForDeletion = true;

    stopGradientDescent();
}

MCV_TsneAnalysis::~MCV_TsneAnalysis()
{
}
