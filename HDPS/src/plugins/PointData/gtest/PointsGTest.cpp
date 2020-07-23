// The file to be tested:
#include <PointData.h>

#include <MainWindow.h>

// GoogleTest header file:
#include <gtest/gtest.h>

namespace
{
    template <typename T>
    void TestPoints(T testFunction)
    {
        int argc = 0;
        QApplication dummyApplication(argc, nullptr);
        hdps::gui::MainWindow mainWindow;
        auto& core = mainWindow.getCore();
        const QString dataName = core.addData("Points", QString("MyTestPointData"));

        testFunction(dynamic_cast<Points&>(core.requestData(dataName)));
    }
}

GTEST_TEST(Points, HasZeroPointsByDefault)
{
    TestPoints([](auto& points)
        {
            ASSERT_EQ(points.getNumPoints(), 0U);
        });
}


GTEST_TEST(Points, HasOneDimensionByDefault)
{
    TestPoints([](auto& points)
        {
            ASSERT_EQ(points.getNumDimensions(), 1U);
        });
}
