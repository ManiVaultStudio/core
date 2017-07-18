#ifndef TSNE_ANALYSIS_H
#define TSNE_ANALYSIS_H

#include "gradient_descent.h"

#include <vector>
#include <string>

class TsneAnalysisPlugin;

class TsneAnalysis
{
public:
    TsneAnalysis();
    ~TsneAnalysis();

    void init(std::string name);

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

    void computeGradientDescent(const TsneAnalysisPlugin& plugin);
    void initTSNE(const std::vector<float>* data, const int numDimensions);
    void initGradientDescent();
    void embed(const TsneAnalysisPlugin& plugin);
    void removePoints();
    void stopGradientDescent();
    void markForDeletion();

    int numDataPoints();

    float radius();

    std::vector<typename atsne::GradientDescent<>::flag_type>* flags();
    std::vector<float>* output();

    std::vector<float>* densityGradientMap();

    inline bool isTsneRunning() { return _isTsneRunning; }
    inline bool isGradientDescentRunning() { return _isGradientDescentRunning; }
    inline bool isMarkedForDeletion() { return _isMarkedForDeletion; }

private:

    void writeTsneChannels();
    void copyFloatOutput();

protected:

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
    float _radius;
    std::vector<typename atsne::GradientDescent<>::flag_type> _flags;
    std::vector<float> _output;
    std::vector<double> _outputDouble;

    KNNSparseMatrix<double> _sparseMatrix;
    std::vector<double> _tSNEData;
    atsne::GradientDescent<> _gradientDescent;
};

#endif // TSNE_ANALYSIS_H
