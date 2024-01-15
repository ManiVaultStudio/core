// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "AnalysisPlugin.h"
#include "Application.h"

namespace mv
{

namespace plugin
{

AnalysisPlugin::AnalysisPlugin(const PluginFactory* factory) :
    Plugin(factory),
    _input(),
    _output()
{
}

void AnalysisPlugin::setObjectName(const QString& name)
{
    QObject::setObjectName("Plugins/Analysis/" + name);
}

void AnalysisPlugin::setInputDataset(const Dataset<DatasetImpl>& inputDataset)
{
    setInputDatasets({ inputDataset });
}

void AnalysisPlugin::setInputDatasets(const Datasets& inputDatasets)
{
    _input = inputDatasets;
}

void AnalysisPlugin::setOutputDataset(const Dataset<DatasetImpl>& outputDataset)
{
    setOutputDatasets({ outputDataset });
}

void AnalysisPlugin::setOutputDatasets(const Datasets& outputDatasets)
{
    _output = outputDatasets;

    for(auto& output : _output)
        output->setAnalysis(this);
}

void AnalysisPlugin::fromVariantMap(const QVariantMap& variantMap)
{
    Plugin::fromVariantMap(variantMap);

    // _input and _output are set before fromVariantMap is called 
    // in PluginManager::requestPlugin()
    // since they might be used in the init() function
}

QVariantMap AnalysisPlugin::toVariantMap() const
{
    auto variantMap = Plugin::toVariantMap();

    QVariantList inputDatasetsGUIDs, outputDatasetsGUIDs;

    for (auto& input : _input)
        inputDatasetsGUIDs << input->getId();

    for (auto& output : _output)
        outputDatasetsGUIDs << output->getId();

    variantMap["inputDatasetsGUIDs"] = inputDatasetsGUIDs;
    variantMap["outputDatasetsGUIDs"] = outputDatasetsGUIDs;

    return variantMap;
}

QIcon AnalysisPluginFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("square-root-alt", color);
}

}

}
