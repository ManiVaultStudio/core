#pragma once

#include "hdi/dimensionality_reduction/sparse_tsne_user_def_probabilities.h"
#include "hdi/dimensionality_reduction/hd_joint_probability_generator.h"

#include <QThread>

#include <vector>
#include <string>

class TsneAnalysis : public QThread
{
    Q_OBJECT
public:
    TsneAnalysis();
    ~TsneAnalysis();

    inline bool verbose() { return _verbose; }
    void setVerbose(bool verbose);
    inline bool skipNormalization() { return _skipNormalization; }
    void setSkipNormalization(bool skipNormalization);
    inline int iterations() { return _iterations; }
    void setIterations(int iterations);
    inline int numTrees() { return _numTrees; }
    void setNumTrees(int numTrees);
    inline int numChecks() { return _numChecks; }
    void setNumChecks(int numChecks);
    inline int exaggerationIter() { return _exaggerationIter; }
    void setExaggerationIter(int exaggerationIter);
    inline int expDecay() { return _expDecay; }
    void setExpDecay(int expDecay);
    inline int perplexity() { return _perplexity; }
    void setPerplexity(int perplexity);
    inline int numDimensionsOutput() { return _numDimensionsOutput; }
    void setNumDimensionsOutput(int numDimensionsOutput);

    void initTSNE(const std::vector<float>& data, const int numDimensions);
    void removePoints();
    void stopGradientDescent();
    void markForDeletion();

    int numDataPoints();

    //std::vector<typename atsne::GradientDescent<>::flag_type>* flags();
    std::vector<float>* output();

    std::vector<float>* densityGradientMap();

    inline bool isTsneRunning() { return _isTsneRunning; }
    inline bool isGradientDescentRunning() { return _isGradientDescentRunning; }
    inline bool isMarkedForDeletion() { return _isMarkedForDeletion; }

private:
    void run() override;

    void computeGradientDescent();
    void initGradientDescent();
    void embed();
    void writeTsneChannels();
    void copyFloatOutput();

signals:
    void newEmbedding();

private:

    // input
    bool _verbose;
    bool _skipNormalization;
    int _iterations;
    int _numTrees;
    int _numChecks;
    int _exaggerationIter;
    int _expDecay;
    int _perplexity;
    int _numDimensionsOutput;
    bool _isGradientDescentRunning;
    bool _isTsneRunning;
    bool _isMarkedForDeletion;

    int _numDataPoints;

    std::vector<float> _densityGradientMap;

    int _continueFromIteration;

    // results
    std::vector<float> _output;

    hdi::dr::HDJointProbabilityGenerator<float>::sparse_scalar_matrix_type _distribution;
    std::vector<float> _tSNEData;

    hdi::dr::SparseTSNEUserDefProbabilities<float> _tsne;

    hdi::data::Embedding<float> _embedding;
};
