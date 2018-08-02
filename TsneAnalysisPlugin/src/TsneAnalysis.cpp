#include "TsneAnalysis.h"

#include <vector>
#include <assert.h>

// TSNE
//#include "vptree.h"
//#include "gradient_descent.h"

//#include "tsne_errors.h"

#include "TsneAnalysisPlugin.h"
#include <QDebug>

#include "hdi/utils/scoped_timers.h"

TsneAnalysis::TsneAnalysis() :
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
_continueFromIteration(0)
{
    
}

void TsneAnalysis::computeGradientDescent()
{
    initGradientDescent();

    embed();
}

void TsneAnalysis::initTSNE(const std::vector<float>& data, const int numDimensions)
{
    _numDataPoints = data.size() / numDimensions;
    // Number of total dimensions
    int numMarkerDimensions = numDimensions;
    qDebug() << "Variables set. Num dims: " << numDimensions << " Num data points: " << _numDataPoints;

    // Input data
    _tSNEData.clear();
    _tSNEData.resize(_numDataPoints * numDimensions);
    qDebug() << "TSNE data allocated.";

    // Fill tSNEData with points * dims
    _tSNEData = std::vector<float>(data.begin(), data.end());
    qDebug() << "Data copied.";
    
    /// Computation of the high dimensional similarities
    _output.resize(_numDataPoints * _numDimensionsOutput);

    qDebug() << "Output allocated.";
    {
        hdi::dr::HDJointProbabilityGenerator<float>::Parameters probGenParams;
        probGenParams._perplexity = _perplexity;
        probGenParams._perplexity_multiplier = 3;
        probGenParams._num_trees = _numTrees;
        probGenParams._num_checks = _numChecks;

        qDebug() << "tSNE initialized.";

        _distribution.clear();
        _distribution.resize(_numDataPoints);
        qDebug() << "Sparse matrix allocated.";

        qDebug() << "Computing high dimensional probability distributions.. Num dims: " << numDimensions << " Num data points: " << _numDataPoints;
        hdi::dr::HDJointProbabilityGenerator<float> probabilityGenerator;
        double t = 0.0;
        {
            hdi::utils::ScopedTimer<double> timer(t);
            probabilityGenerator.computeJointProbabilityDistribution(_tSNEData.data(), numDimensions, _numDataPoints, _distribution, probGenParams);
        }
        qDebug() << "Probability distributions calculated.";
        qDebug() << "================================================================================";
        qDebug() << "A-tSNE: Compute probability distribution: " << t / 1000 << " seconds";
        qDebug() << "--------------------------------------------------------------------------------";
    }
}

void TsneAnalysis::initGradientDescent()
{
    _continueFromIteration = 0;

    _isTsneRunning = true;

    hdi::dr::SparseTSNEUserDefProbabilities<float>::Parameters tsneParams;

    tsneParams._embedding_dimensionality = _numDimensionsOutput;
    tsneParams._mom_switching_iter = _exaggerationIter;
    tsneParams._remove_exaggeration_iter = _exaggerationIter;
    tsneParams._exponential_decay_iter = 150;
    tsneParams._exaggeration_factor = 10 + _numDataPoints / 5000.;

    _tsne.setTheta(std::min(0.5, std::max(0.0, (_numDataPoints - 1000.0)*0.00005)));

    _tsne.initialize(_distribution, &_embedding, tsneParams);

    copyFloatOutput();
}

// Computing gradient descent
void TsneAnalysis::embed()
{

    double t = 0.0;
    double kld = 0.0;
    double kldmin = 0.0;
    double kldmax = 0.0;
    {
        
        qDebug() << "A-tSNE: Computing gradient descent..\n";
        _isGradientDescentRunning = true;

        // Performs gradient descent for every iteration
        for (int iter = 0; iter < _iterations; ++iter){
            hdi::utils::ScopedTimer<double> timer(t);
            if (!_isGradientDescentRunning)
            {
                _continueFromIteration = iter;
                break;
            }

            _tsne.doAnIteration();
            //nut::SecureLogValue(&_log, "Iter", iter, _verbose);

            copyFloatOutput();

            //if ((iter + 1) % 100 == 0){
            //    TSNEErrorUtils<>::ComputeBarnesHutTSNEErrorWithTreeComputation(_sparseMatrix, _outputDouble.data(), 2, kld, kldmin, kldmax, _gradientDescent._param._theta);
            //    qDebug() << "tSNE Analysis" << ": iteration: " << iter + 1 << ", KL-divergence: " << kld;
            //}

            emit newEmbedding();

            qDebug() << "Time: " << t;
        }
        
        _isGradientDescentRunning = false;
        _isTsneRunning = false;
    }
    //TSNEErrorUtils<>::ComputeBarnesHutTSNEErrorWithTreeComputation(_sparseMatrix, _outputDouble.data(), 2, kld, kldmin, kldmax, _gradientDescent._param._theta);
    qDebug() << "--------------------------------------------------------------------------------";
    qDebug() << "A-tSNE: Finished embedding of " << "tSNE Analysis" << " in: " << t / 1000 << " seconds " << ".KL - divergence is : " << kld;
    qDebug() << "================================================================================";
}

void TsneAnalysis::run() {
    computeGradientDescent();
}

// Copy tSNE output to our output
void TsneAnalysis::copyFloatOutput()
{
    _output.assign(_embedding.getContainer().begin(), _embedding.getContainer().end());
}

void TsneAnalysis::removePoints()
{
}

int TsneAnalysis::numDataPoints()
{
    return _numDataPoints;
}

//std::vector<typename atsne::GradientDescent<>::flag_type>* TsneAnalysis::flags()
//{
//    return &_flags;
//}

std::vector<float>* TsneAnalysis::output()
{
    return &_output;
}

std::vector<float>* TsneAnalysis::densityGradientMap()
{
    return &_densityGradientMap;
}

void TsneAnalysis::setVerbose(bool verbose)
{
    _verbose = verbose;
}

void TsneAnalysis::setSkipNormalization(bool skipNormalization)
{
    _skipNormalization = skipNormalization;
}

void TsneAnalysis::setIterations(int iterations)
{
    _iterations = iterations;
}

void TsneAnalysis::setNumTrees(int numTrees)
{
    _numTrees = numTrees;
}

void TsneAnalysis::setNumChecks(int numChecks)
{
    _numChecks = numChecks;
}

void TsneAnalysis::setExaggerationIter(int exaggerationIter)
{
    _exaggerationIter = exaggerationIter;
}

void TsneAnalysis::setExpDecay(int expDecay)
{
    _expDecay = expDecay;
}

void TsneAnalysis::setPerplexity(int perplexity)
{
    _perplexity = perplexity;
}

void TsneAnalysis::setNumDimensionsOutput(int numDimensionsOutput)
{
    _numDimensionsOutput = numDimensionsOutput;
}

void TsneAnalysis::stopGradientDescent()
{
    _isGradientDescentRunning = false;
}

void TsneAnalysis::markForDeletion()
{
    _isMarkedForDeletion = true;

    stopGradientDescent();
}

TsneAnalysis::~TsneAnalysis()
{
}
