# CMakeLists to generate a project that has all subprojects of the plugin system in one.

cmake_minimum_required(VERSION 3.1.0)

set(PROJECT "HdpsAllInOne")

PROJECT(${PROJECT})

add_subdirectory(Clusters)
add_subdirectory(CsvLoader)
add_subdirectory(ExampleDataPlugin)
add_subdirectory(ExampleViewPlugin)
add_subdirectory(FcsLoader)
add_subdirectory(BinLoader)
add_subdirectory(HDPS)
add_subdirectory(HeatMap)
add_subdirectory(MeanShift)
add_subdirectory(Scatterplot)
add_subdirectory(SpadeAnalysis)

add_dependencies(ClustersPlugin HDPS)
add_dependencies(ClustersPlugin PointsPlugin)
add_dependencies(PointsPlugin HDPS)
add_dependencies(ExampleDataPlugin HDPS)
add_dependencies(ExampleDataPlugin PointsPlugin)
add_dependencies(ExampleViewPlugin HDPS)
add_dependencies(ExampleViewPlugin PointsPlugin)
add_dependencies(ScatterplotPlugin HDPS)
add_dependencies(ScatterplotPlugin PointsPlugin)
add_dependencies(CsvLoader HDPS)
add_dependencies(CsvLoader PointsPlugin)
add_dependencies(FcsLoader HDPS)
add_dependencies(FcsLoader PointsPlugin)
add_dependencies(BinLoader HDPS)
add_dependencies(BinLoader PointsPlugin)
add_dependencies(MeanShiftClusteringPlugin HDPS)
add_dependencies(MeanShiftClusteringPlugin ClustersPlugin)
add_dependencies(MeanShiftClusteringPlugin PointsPlugin)
add_dependencies(HeatMapPlugin ClustersPlugin)
add_dependencies(HeatMapPlugin PointsPlugin)
add_dependencies(SpadeAnalysisPlugin ClustersPlugin)
add_dependencies(SpadeAnalysisPlugin PointsPlugin)

# Note: The plugins BasicClustering and ImageView are not in used (as Julian Thijssen commented,
# June 24, 2019), so this CMakeLists does not add those subdirectories.

# On the master branch, you may also do add_subdirectory(HDVolLoader)

