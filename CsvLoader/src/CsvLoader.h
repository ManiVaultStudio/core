#pragma once

#include <LoaderPlugin.h>

#include "InputDialog.h"

#include <QObject>
#include <QStringList>

#include <vector>

using namespace hdps::plugin;

// =============================================================================
// View
// =============================================================================

class CsvLoader : public QObject, public LoaderPlugin
{
    Q_OBJECT
public:
    CsvLoader() : LoaderPlugin("CSV Loader") { }
    ~CsvLoader(void) override;
    
    void init() override;

    void loadData() Q_DECL_OVERRIDE;

public slots:
    void dialogClosed(QString dataSetName, bool hasHeaders);

private:
    std::vector<QStringList> _loadedData;
};


// =============================================================================
// Factory
// =============================================================================

class CsvLoaderFactory : public LoaderPluginFactory
{
    Q_INTERFACES(hdps::plugin::LoaderPluginFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.tudelft.CsvLoader"
                      FILE  "CsvLoader.json")
    
public:
    CsvLoaderFactory(void) {}
    ~CsvLoaderFactory(void) override {}
    
    LoaderPlugin* produce() override;
};
