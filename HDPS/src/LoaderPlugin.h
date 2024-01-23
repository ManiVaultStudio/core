// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#ifndef HDPS_LOADERPLUGIN_H
#define HDPS_LOADERPLUGIN_H

#include "Plugin.h"
#include "DataHierarchyItem.h"
#include "Application.h"

#include <QString>

#include <exception>

namespace mv::plugin {

struct DataLoadException : public std::exception
{
public:
    DataLoadException(QString filePath, QString reason)
        : _filePath(filePath),
        _reason(reason)
    {
        _what = ("Failed to load file at: " + _filePath + "\nReason: " + _reason).toStdString();
    }

    const char* what() const throw () override
    {
        return _what.c_str();
    }

private:
    QString _filePath;
    QString _reason;
    std::string _what;
};


class LoaderPlugin : public Plugin
{
public:
    LoaderPlugin(const PluginFactory* factory) : Plugin(factory) { }

    ~LoaderPlugin() override {};

    /**
     * Should be implemented by loader plugins to parse their specific data.
     * This function will be called when the user clicks on the menu item for this loader.
     * The implementation is free to create file dialogs if desired.
     */
    virtual void loadData() = 0;

protected:
    /**
     * Asks the user for the name of a file to load data from. Opens a file dialog at the
     * directory of the last loaded file. The path of this directory is stored with the
     * application settings (in the registry, on Windows).
     * `fileNameFilter` could be something like "Text Files (*.txt)".
     */
    QString AskForFileName(const QString& fileNameFilter);
};

/**
 * Loader plugin factory class
 *
 * Class which produces instances of loader plugins
 */
class LoaderPluginFactory : public PluginFactory
{
    Q_OBJECT

public:

    /** Default constructor which sets the plugin factory type to loader */
    LoaderPluginFactory() :
        PluginFactory(Type::LOADER)
    {
    }

    /**
     * Get plugin icon
     * @return Plugin icon
     */
    QIcon getIcon() const override {
        return Application::getIconFont("FontAwesome").getIcon("file-import");
    }

    /**
     * Get plugin category (loader/writer/transformation etc.) icon
     * @return Icon which reflects to the plugin factory category
     */
    QIcon getCategoryIcon() const override {
        return Application::getIconFont("FontAwesome").getIcon("file-import");
    }

    /** Produces an instance of a loader plugin */
    LoaderPlugin* produce() override = 0;

    /**
     * Get plugin trigger actions given \p datasets
     * @param datasets Vector of input datasets
     * @return Vector of plugin trigger actions
     */
    gui::PluginTriggerActions getPluginTriggerActions(const mv::Datasets& datasets) const override;

    /**
     * Get plugin trigger actions given \p dataTypes
     * @param datasetTypes Vector of input data types
     * @return Vector of plugin trigger actions
     */
    gui::PluginTriggerActions getPluginTriggerActions(const mv::DataTypes& dataTypes) const override;
};

}

Q_DECLARE_INTERFACE(mv::plugin::LoaderPluginFactory, "hdps.LoaderPluginFactory")

#endif // HDPS_LOADERPLUGIN_H
