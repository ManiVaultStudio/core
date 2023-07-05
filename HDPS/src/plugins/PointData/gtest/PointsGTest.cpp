// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

// The file to be tested:
#include <PointData.h>

#include <MainWindow.h>

// GoogleTest header file:
#include <gtest/gtest.h>

#include <random>


namespace
{
    template <typename T>
    void testCore(T testCoreFunction)
    {
        int argc = 0;
        QApplication dummyApplication(argc, nullptr);
        hdps::gui::MainWindow mainWindow;
        testCoreFunction(mainWindow.getCore());
    }


    auto& addPointsToCore(hdps::CoreInterface& core, const char* const name)
    {
        return dynamic_cast<Points&>(core.requestData(
            core.addData(QString::fromLatin1("Points"), QString::fromLatin1(name))));
    }

    template <std::size_t N, typename T>
    auto generateRandomData(const T minValue, const T maxValue)
    {
        std::vector<T> result(N);

        std::mt19937 randomNumberEngine;

        std::generate(begin(result), end(result), [&randomNumberEngine, minValue, maxValue]
            {
                std::uniform_int_distribution<T> distribution(minValue, maxValue);

                return distribution(randomNumberEngine);
            });
        return result;
    }
}


GTEST_TEST(Points, hasZeroPointsAndOneDimensionByDefault)
{
    testCore([](hdps::CoreInterface& core)
        {
            const auto& defaultPoints = addPointsToCore(core, "defaultPoints");

            ASSERT_EQ(defaultPoints.getNumPoints(), 0U);
            ASSERT_EQ(defaultPoints.getNumDimensions(), 1U);
        });
}


GTEST_TEST(Points, visitDataAllowsAccessToAnyRandomSubset)
{
    testCore([](hdps::CoreInterface& core)
        {
            auto& fullPoints = addPointsToCore(core, "fullPoints");

            // As an example test case, try the following type and numbers, for the input data:
            using PointDataElementType = std::uint16_t;
            const auto numberOfDimensions = 3;
            const auto numberOfPoints = 42;

            const auto numberOfDataElements = numberOfDimensions * numberOfPoints;

            // Create a (pseudo) random full data set:
            const auto inputData =
                generateRandomData<numberOfDataElements, PointDataElementType>(1, std::numeric_limits<PointDataElementType>::max());
            fullPoints.setData(inputData, numberOfDimensions);

            // Create a (pseudo) random subset, having half the number of points a s the full set:
            auto& subsetPoints = dynamic_cast<Points&>(core.requestData(fullPoints.createSubset()));
            subsetPoints.indices =
                generateRandomData<numberOfDataElements / 2, unsigned>(0, numberOfPoints - 1);

            // Create an output buffer for the same number of data elements as the full input set. 
            std::vector<double> outputData(inputData.size());

            subsetPoints.visitData([&outputData](auto subset)
            {
                for (auto pointView : subset)
                {
                    const auto numberOfDimensions = pointView.size();

                    // Store the i-th point from the full input set at the
                    // corresponding location in the output buffer. 
                    auto dataValueIndex = pointView.index() * numberOfDimensions;

                    for (const auto value : pointView)
                    {
                        outputData[dataValueIndex] = value;
                        ++dataValueIndex;
                    }
                }
            });

            const auto beginOfIndices = begin(subsetPoints.indices);
            const auto endOfIndices = end(subsetPoints.indices);

            // Assert that for each point in the subset, its values are copied
            // to the corresponding location in the output, while each other
            // output value is still zero. 

            for (unsigned pointIndex{}; pointIndex < numberOfPoints; ++pointIndex)
            {
                const bool isPointInSubset = std::find(beginOfIndices, endOfIndices, pointIndex) != endOfIndices;

                for (unsigned dimensionIndex{}; dimensionIndex < numberOfDimensions; ++dimensionIndex)
                {
                    const auto valueIndex = (pointIndex * numberOfDimensions) + dimensionIndex;
                    const auto outputValue = outputData[valueIndex];

                    if (isPointInSubset)
                    {
                        ASSERT_EQ(outputValue, fullPoints.getValueAt(valueIndex));
                    }
                    else
                    {
                        ASSERT_EQ(outputValue, 0.0);
                    }
                }
            }
        });
}
