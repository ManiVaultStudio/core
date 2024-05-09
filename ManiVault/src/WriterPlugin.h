// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

/**
* Writer.h
*
* Base plugin class for plugins that write data to a file.
*/

#include "Application.h"
#include "Plugin.h"

#include <QString>

using namespace mv::util;

namespace mv
{
namespace plugin
{

class CORE_EXPORT WriterPlugin : public Plugin
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


class CORE_EXPORT WriterPluginFactory : public PluginFactory
{
    Q_OBJECT
    
public:
    WriterPluginFactory() :
        PluginFactory(Type::WRITER)
    {
    }

    ~WriterPluginFactory() = default;

    /**
     * Set name of the object
     * @param name Name of the object
     */
    void setObjectName(const QString& name)
    {
        QObject::setObjectName("Plugins/Writer/" + name);
    }

    /**
     * Get plugin icon
     * @param color Icon color for flat (font) icons
     * @return Icon
     */
    QIcon getIcon(const QColor& color = Qt::black) const override {
        return Application::getIconFont("FontAwesome").getIcon("file-export", color);
    }

    /**
     * Get plugin category (loader/writer/transformation etc.) icon
     * @return Icon which belongs to the plugin factory category
     */
    QIcon getCategoryIcon() const override;

    /**
    * Produces an instance of a writer plugin. This function gets called by the plugin manager.
    */
    WriterPlugin* produce() override = 0;
};

} // namespace plugin

} // namespace mv

Q_DECLARE_INTERFACE(mv::plugin::WriterPluginFactory, "ManiVault.WriterPluginFactory")
