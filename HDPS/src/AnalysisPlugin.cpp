// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "AnalysisPlugin.h"
#include "Application.h"

namespace hdps
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

void AnalysisPlugin::setInputDataset(Dataset<DatasetImpl> inputDataset)
{
    _input = inputDataset;
}

void AnalysisPlugin::setOutputDataset(Dataset<DatasetImpl> outputDataset)
{
    _output = outputDataset;
}

QIcon AnalysisPluginFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("square-root-alt", color);
}

}

}
