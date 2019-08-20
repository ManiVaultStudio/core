# CMakeLists to generate a project that has all subprojects of the plugin system in one.

cmake_minimum_required(VERSION 3.1.0)

set(PROJECT "HdpsAllInOne")

PROJECT(${PROJECT})

add_subdirectory(Clusters)
add_subdirectory(CsvLoader)
add_subdirectory(ExampleDataPlugin)
add_subdirectory(ExampleViewPlugin)
add_subdirectory(FcsLoader)
add_subdirectory(HDPS)
add_subdirectory(HeatMap)
add_subdirectory(MeanShift)
add_subdirectory(Scatterplot)
add_subdirectory(SpadeAnalysis)
add_subdirectory(TsneAnalysis)

add_dependencies(HeatMapPlugin ClustersPlugin)
add_dependencies(SpadeAnalysisPlugin ClustersPlugin)

# Note: The plugins BasicClustering and ImageView are not in used (as Julian Thijssen commented,
# June 24, 2019), so this CMakeLists does not add those subdirectories.

# On the master branch, you may also do add_subdirectory(HDVolLoader)

