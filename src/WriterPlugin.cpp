// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WriterPlugin.h"

using namespace mv::util;

namespace mv::plugin
{

WriterPlugin::WriterPlugin(const PluginFactory* factory) :
    Plugin(factory)
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

}
