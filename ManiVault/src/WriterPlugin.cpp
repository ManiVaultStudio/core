// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WriterPlugin.h"

using namespace mv::util;

namespace mv
{
namespace plugin
{

WriterPlugin::WriterPlugin(const PluginFactory* factory) :
    Plugin(factory),
    _input()
{

}

void WriterPlugin::setInputDataset(const Dataset<DatasetImpl>& inputDataset)
{
    setInputDatasets({ inputDataset });
}

void WriterPlugin::setInputDatasets(const Datasets& inputDatasets)
{
    _input = inputDatasets;
}

StyledIcon WriterPluginFactory::getIcon() const
{
	return util::StyledIcon("file-export");
}

StyledIcon WriterPluginFactory::getCategoryIcon() const
{
    return StyledIcon("file-export");
}

}

}
