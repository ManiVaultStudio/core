// SPDX-License-Identifier: LGPL-3.0-or-later 
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

using namespace hdps::util;

namespace hdps
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
    void setInputDataset(Dataset<DatasetImpl> inputDataset);

    /** Get input dataset smart pointer */
    template<typename DatasetType = DatasetImpl>
    Dataset<DatasetType> getInputDataset() {
        return Dataset<DatasetType>(_input.get<DatasetType>());
    }

protected:
    Dataset<DatasetImpl>    _input;     /** Input dataset smart pointer */
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

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::WriterPluginFactory, "hdps.WriterPluginFactory")

#endif // HDPS_WRITERPLUGIN_H
