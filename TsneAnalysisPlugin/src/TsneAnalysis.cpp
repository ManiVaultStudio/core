#include "TsneAnalysis.h"

#include <vector>
#include <assert.h>

// TSNE
#include "vptree.h"
#include "gradient_descent.h"
#include "tsne_errors.h"

#include "TsneAnalysisPlugin.h"
#include <QDebug>

#include "timings/scoped_timers.h"

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
_continueFromIteration(0),
_radius(0.0f)
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
    _tSNEData = std::vector<double>(data.begin(), data.end());
    qDebug() << "Data copied.";
    
    /// Computation of the high dimensional similarities
    _output.resize(_numDataPoints * _numDimensionsOutput);
    _outputDouble.resize(_numDataPoints * _numDimensionsOutput);
    qDebug() << "Output allocated.";
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
        qDebug() << "Computing high dimensional probability distributions.. Num dims: " << numDimensions << " Num data points: " << _numDataPoints;
        double t = 0.0;
        {
            nut::ScopedTimer<double> timer(t);
            initializer.ComputeHighDimensionalProbDistributions(_tSNEData.data(), _numDataPoints, numDimensions, _sparseMatrix);
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

            if ((iter + 1) % 100 == 0){
                TSNEErrorUtils<>::ComputeBarnesHutTSNEErrorWithTreeComputation(_sparseMatrix, _outputDouble.data(), 2, kld, kldmin, kldmax, _gradientDescent._param._theta);
                qDebug() << "tSNE Analysis" << ": iteration: " << iter + 1 << ", KL-divergence: " << kld;
            }

            emit newEmbedding();

            qDebug() << "Time: " << t;
        }
        
        _isGradientDescentRunning = false;
        _isTsneRunning = false;
    }
    TSNEErrorUtils<>::ComputeBarnesHutTSNEErrorWithTreeComputation(_sparseMatrix, _outputDouble.data(), 2, kld, kldmin, kldmax, _gradientDescent._param._theta);
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
    _output.assign(_outputDouble.begin(), _outputDouble.end());
}

void TsneAnalysis::removePoints()
{
}

int TsneAnalysis::numDataPoints()
{
    return _numDataPoints;
}

float TsneAnalysis::radius()
{
    return _radius;
}

std::vector<typename atsne::GradientDescent<>::flag_type>* TsneAnalysis::flags()
{
    return &_flags;
}

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
