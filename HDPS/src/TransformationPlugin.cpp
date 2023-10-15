// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TransformationPlugin.h"
#include "Application.h"

namespace mv
{

namespace plugin
{

TransformationPlugin::TransformationPlugin(const PluginFactory* factory) :
    Plugin(factory),
    _inputDatasets()
{

}

mv::Datasets TransformationPlugin::getInputDatasets() const
{
    return _inputDatasets;
}

void TransformationPlugin::setInputDatasets(const Datasets& inputDatasets)
{
    _inputDatasets = inputDatasets;
}

TransformationPluginFactory::TransformationPluginFactory() :
    PluginFactory(Type::TRANSFORMATION)
{

}

QIcon TransformationPluginFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("random", color);
}

}
}
