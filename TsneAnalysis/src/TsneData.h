#pragma once

#include <vector>
#include <cassert>

class TsneData
{
public:
    unsigned int getNumPoints() const
    {
        return _numPoints;
    }

    unsigned int getNumDimensions() const
    {
        return _numDimensions;
    }

    const std::vector<float>& getData() const
    {
        return _data;
    }

    // Just here because computeJointProbabilityDistribution doesn't take a const vector
    std::vector<float>& getDataNonConst()
    {
        return _data;
    }

    void assign(unsigned int numPoints, unsigned int numDimensions, const std::vector<float>& inputData)
    {
        assert(inputData.size() == numPoints * numDimensions);

        _numPoints = numPoints;
        _numDimensions = numDimensions;
        _data = inputData;
    }

private:
    unsigned int _numPoints;
    unsigned int _numDimensions;

    std::vector<float> _data;
};
