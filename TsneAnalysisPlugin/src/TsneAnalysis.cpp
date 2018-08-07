#include "TsneAnalysis.h"

#include <vector>
#include <assert.h>

#include "TsneAnalysisPlugin.h"
#include <QDebug>

#include "hdi/utils/scoped_timers.h"

TsneAnalysis::TsneAnalysis() :
_iterations(1000),
_numTrees(4),
_numChecks(1024),
_exaggerationIter(250),
_perplexity(30),
_numDimensionsOutput(2),
_verbose(false),
_isGradientDescentRunning(false),
_isTsneRunning(false),
_isMarkedForDeletion(false),
_continueFromIteration(0)
{
    
}

TsneAnalysis::~TsneAnalysis()
{
}

void TsneAnalysis::computeGradientDescent()
{
    initGradientDescent();

    embed();
}

void TsneAnalysis::initTSNE(const std::vector<float>& data, const int numDimensions)
{
    unsigned int numPoints = data.size() / numDimensions;
    qDebug() << "Variables set. Num dims: " << numDimensions << " Num data points: " << numPoints;
    
    // Input data
    _inputData.assign(numPoints, numDimensions, data);
    qDebug() << "t-SNE input data assigned.";
    
    /// Computation of the high dimensional similarities
    qDebug() << "Output allocated.";
    {
        hdi::dr::HDJointProbabilityGenerator<float>::Parameters probGenParams;
        probGenParams._perplexity = _perplexity;
        probGenParams._perplexity_multiplier = 3;
        probGenParams._num_trees = _numTrees;
        probGenParams._num_checks = _numChecks;

        qDebug() << "tSNE initialized.";

        _probabilityDistribution.clear();
        _probabilityDistribution.resize(numPoints);
        qDebug() << "Sparse matrix allocated.";

        qDebug() << "Computing high dimensional probability distributions.. Num dims: " << numDimensions << " Num data points: " << numPoints;
        hdi::dr::HDJointProbabilityGenerator<float> probabilityGenerator;
        double t = 0.0;
        {
            hdi::utils::ScopedTimer<double> timer(t);
            probabilityGenerator.computeJointProbabilityDistribution(_inputData.getDataNonConst().data(), numDimensions, numPoints, _probabilityDistribution, probGenParams);
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
    tsneParams._exaggeration_factor = 10 + _inputData.getNumPoints() / 5000.;
    _gradientDescent.setTheta(std::min(0.5, std::max(0.0, (_inputData.getNumPoints() - 1000.0)*0.00005)));

    _gradientDescent.initialize(_probabilityDistribution, &_embedding, tsneParams);

    copyFloatOutput();
}

// Computing gradient descent
void TsneAnalysis::embed()
{
    double t = 0.0;
    {
        
        qDebug() << "A-tSNE: Computing gradient descent..\n";
        _isGradientDescentRunning = true;

        // Performs gradient descent for every iteration
        for (int iter = 0; iter < _iterations; ++iter)
        {
            hdi::utils::ScopedTimer<double> timer(t);
            if (!_isGradientDescentRunning)
            {
                _continueFromIteration = iter;
                break;
            }

            _gradientDescent.doAnIteration();
            copyFloatOutput();
            emit newEmbedding();

            qDebug() << "Time: " << t;
        }
        
        _isGradientDescentRunning = false;
        _isTsneRunning = false;
    }

    qDebug() << "--------------------------------------------------------------------------------";
    qDebug() << "A-tSNE: Finished embedding of " << "tSNE Analysis" << " in: " << t / 1000 << " seconds ";
    qDebug() << "================================================================================";
}

void TsneAnalysis::run() {
    computeGradientDescent();
}

// Copy tSNE output to our output
void TsneAnalysis::copyFloatOutput()
{
    _outputData.assign(_inputData.getNumPoints(), _numDimensionsOutput, _embedding.getContainer());
}

const TsneData& TsneAnalysis::output()
{
    return _outputData;
}

void TsneAnalysis::setVerbose(bool verbose)
{
    _verbose = verbose;
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
