// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#ifndef HDPS_WRITERPLUGIN_H
#define HDPS_WRITERPLUGIN_H
/**
* Writer.h
*
* Base plugin class for plugins that write data to a file.
*/


#include "Plugin.h"

#include <QString>

using namespace mv::util;

namespace mv
{
namespace plugin
{

class WriterPlugin : public Plugin
{
public:
    WriterPlugin(const PluginFactory* factory);

    ~WriterPlugin() override {};

    virtual void writeData() = 0;

public:

    /**
     * Set input dataset smart pointer
     * @param inputDataset Smart pointer to input dataset
     */
    void setInputDataset(const Dataset<DatasetImpl>& inputDataset);

    /**
     * Set input datasets smart pointers
     * @param inputDatasets Vector of smart pointers to the input datasets
     */
    void setInputDatasets(const Datasets& inputDatasets);

    /** Get input dataset smart pointer */
    template<typename DatasetType = DatasetImpl>
    Dataset<DatasetType> getInputDataset() {
        if (_input.size() > 0)
            return Dataset<DatasetType>(_input[0].get<DatasetType>());
        else
            return Dataset<DatasetImpl>();
    }

    /** Get input dataset smart pointer vector */
    Datasets getInputDatasets() {
        return _input;
    }

protected:
    Datasets    _input;     /** Input datasets smart pointers */
};


class WriterPluginFactory : public PluginFactory
{
    Q_OBJECT
    
public:
    WriterPluginFactory();

    ~WriterPluginFactory() override {};

    /**
     * Get plugin icon
     * @param color Icon color for flat (font) icons
     * @return Icon
     */
    QIcon getIcon(const QColor& color = Qt::black) const override;

    WriterPlugin* produce() override = 0;
};

} // namespace plugin

} // namespace mv

Q_DECLARE_INTERFACE(mv::plugin::WriterPluginFactory, "hdps.WriterPluginFactory")

#endif // HDPS_WRITERPLUGIN_H