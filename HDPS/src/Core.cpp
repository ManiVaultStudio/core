#include "Core.h"

#include "MainWindow.h"
#include "PluginManager.h"
#include "DataManager.h"

#include "exceptions/SetNotFoundException.h"

#include "AnalysisPlugin.h"
#include "LoaderPlugin.h"
#include "WriterPlugin.h"
#include "ViewPlugin.h"
#include "RawData.h"
#include "Set.h"
#include "DataConsumer.h"

#include <QMessageBox>

#ifndef __GNUC__

#ifdef __clang__
# error "__clang__ should imply __GNUC__"
#endif

/*
No Clang compile, because of https://travis-ci.com/bldrvnlw/conan-hdps-core/jobs/263538710

cd /Users/travis/.conan/data/hdps-core/0.1.0/lkeb/stable/build/9294abb7efc47eb9a928ee29d3d66b71528142ba/HDPS && /Applications/Xcode-10.3.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  -DHDPS_Private_EXPORTS -DQT_CORE_LIB -DQT_GUI_LIB -DQT_MESSAGELOGCONTEXT -DQT_NETWORK_LIB -DQT_NO_DEBUG -DQT_POSITIONING_LIB -DQT_PRINTSUPPORT_LIB -DQT_QML_LIB -DQT_QUICK_LIB -DQT_WEBCHANNEL_LIB -DQT_WEBENGINECORE_LIB -DQT_WEBENGINEWIDGETS_LIB -DQT_WIDGETS_LIB -I/Users/travis/.conan/data/hdps-core/0.1.0/lkeb/stable/build/9294abb7efc47eb9a928ee29d3d66b71528142ba/HDPS -I/Users/travis/.conan/data/hdps-core/0.1.0/lkeb/stable/build/9294abb7efc47eb9a928ee29d3d66b71528142ba/core/HDPS -I/Users/travis/.conan/data/hdps-core/0.1.0/lkeb/stable/build/9294abb7efc47eb9a928ee29d3d66b71528142ba/HDPS/HDPS_Private_autogen/include -iframework /Users/travis/.conan/data/qt/5.12.4/lkeb/stable/package/ab922693f8b78138fd6870b7eb238585bdde51bc/lib -isystem /Users/travis/.conan/data/qt/5.12.4/lkeb/stable/package/ab922693f8b78138fd6870b7eb238585bdde51bc/lib/QtWidgets.framework/Headers -isystem /Users/travis/.conan/data/qt/5.12.4/lkeb/stable/package/ab922693f8b78138fd6870b7eb238585bdde51bc/lib/QtGui.framework/Headers -isystem /Applications/Xcode-10.3.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.14.sdk/System/Library/Frameworks/OpenGL.framework/Headers -isystem /Users/travis/.conan/data/qt/5.12.4/lkeb/stable/package/ab922693f8b78138fd6870b7eb238585bdde51bc/lib/QtCore.framework/Headers -isystem /Users/travis/.conan/data/qt/5.12.4/lkeb/stable/package/ab922693f8b78138fd6870b7eb238585bdde51bc/./mkspecs/macx-clang -isystem /Users/travis/.conan/data/qt/5.12.4/lkeb/stable/package/ab922693f8b78138fd6870b7eb238585bdde51bc/lib/QtWebEngineWidgets.framework/Headers -isystem /Users/travis/.conan/data/qt/5.12.4/lkeb/stable/package/ab922693f8b78138fd6870b7eb238585bdde51bc/lib/QtWebEngineCore.framework/Headers -isystem /Users/travis/.conan/data/qt/5.12.4/lkeb/stable/package/ab922693f8b78138fd6870b7eb238585bdde51bc/lib/QtQuick.framework/Headers -isystem /Users/travis/.conan/data/qt/5.12.4/lkeb/stable/package/ab922693f8b78138fd6870b7eb238585bdde51bc/lib/QtQml.framework/Headers -isystem /Users/travis/.conan/data/qt/5.12.4/lkeb/stable/package/ab922693f8b78138fd6870b7eb238585bdde51bc/lib/QtNetwork.framework/Headers -isystem /Users/travis/.conan/data/qt/5.12.4/lkeb/stable/package/ab922693f8b78138fd6870b7eb238585bdde51bc/lib/QtWebChannel.framework/Headers -isystem /Users/travis/.conan/data/qt/5.12.4/lkeb/stable/package/ab922693f8b78138fd6870b7eb238585bdde51bc/lib/QtPositioning.framework/Headers -isystem /Users/travis/.conan/data/qt/5.12.4/lkeb/stable/package/ab922693f8b78138fd6870b7eb238585bdde51bc/lib/QtPrintSupport.framework/Headers  -O3 -DNDEBUG -isysroot /Applications/Xcode-10.3.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.14.sdk -fPIC   -fPIC -std=c++1z -o CMakeFiles/HDPS_Private.dir/src/Core.cpp.o -c /Users/travis/.conan/data/hdps-core/0.1.0/lkeb/stable/build/9294abb7efc47eb9a928ee29d3d66b71528142ba/core/HDPS/src/Core.cpp

/Users/travis/.conan/data/hdps-core/0.1.0/lkeb/stable/build/9294abb7efc47eb9a928ee29d3d66b71528142ba/core/HDPS/src/Core.cpp:18:10: fatal error: 'execution' file not found

No GCC build because of https://travis-ci.com/bldrvnlw/conan-hdps-core/jobs/263554477 
cd /home/conan/.conan/data/hdps-core/0.1.0/lkeb/stable/build/05b936f4a20297a72984f1cfeff9f028e5d42576/HDPS && /usr/bin/g++  -DHDPS_Private_EXPORTS -DQT_CORE_LIB -DQT_GUI_LIB -DQT_MESSAGELOGCONTEXT -DQT_NETWORK_LIB -DQT_NO_DEBUG -DQT_POSITIONING_LIB -DQT_PRINTSUPPORT_LIB -DQT_QML_LIB -DQT_QUICK_LIB -DQT_WEBCHANNEL_LIB -DQT_WEBENGINECORE_LIB -DQT_WEBENGINEWIDGETS_LIB -DQT_WIDGETS_LIB -I/home/conan/.conan/data/hdps-core/0.1.0/lkeb/stable/build/05b936f4a20297a72984f1cfeff9f028e5d42576/HDPS -I/home/conan/.conan/data/hdps-core/0.1.0/lkeb/stable/build/05b936f4a20297a72984f1cfeff9f028e5d42576/core/HDPS -I/home/conan/.conan/data/hdps-core/0.1.0/lkeb/stable/build/05b936f4a20297a72984f1cfeff9f028e5d42576/HDPS/HDPS_Private_autogen/include -isystem /home/conan/.conan/data/qt/5.12.4/lkeb/stable/package/e46c6e02f2760cd2e42160a54432cf1e7354d4ec/include -isystem /home/conan/.conan/data/qt/5.12.4/lkeb/stable/package/e46c6e02f2760cd2e42160a54432cf1e7354d4ec/include/QtWidgets -isystem /home/conan/.conan/data/qt/5.12.4/lkeb/stable/package/e46c6e02f2760cd2e42160a54432cf1e7354d4ec/include/QtGui -isystem /home/conan/.conan/data/qt/5.12.4/lkeb/stable/package/e46c6e02f2760cd2e42160a54432cf1e7354d4ec/include/QtCore -isystem /home/conan/.conan/data/qt/5.12.4/lkeb/stable/package/e46c6e02f2760cd2e42160a54432cf1e7354d4ec/./mkspecs/linux-g++ -isystem /home/conan/.conan/data/qt/5.12.4/lkeb/stable/package/e46c6e02f2760cd2e42160a54432cf1e7354d4ec/include/QtWebEngineWidgets -isystem /home/conan/.conan/data/qt/5.12.4/lkeb/stable/package/e46c6e02f2760cd2e42160a54432cf1e7354d4ec/include/QtWebEngineCore -isystem /home/conan/.conan/data/qt/5.12.4/lkeb/stable/package/e46c6e02f2760cd2e42160a54432cf1e7354d4ec/include/QtQuick -isystem /home/conan/.conan/data/qt/5.12.4/lkeb/stable/package/e46c6e02f2760cd2e42160a54432cf1e7354d4ec/include/QtQml -isystem /home/conan/.conan/data/qt/5.12.4/lkeb/stable/package/e46c6e02f2760cd2e42160a54432cf1e7354d4ec/include/QtNetwork -isystem /home/conan/.conan/data/qt/5.12.4/lkeb/stable/package/e46c6e02f2760cd2e42160a54432cf1e7354d4ec/include/QtWebChannel -isystem /home/conan/.conan/data/qt/5.12.4/lkeb/stable/package/e46c6e02f2760cd2e42160a54432cf1e7354d4ec/include/QtPositioning -isystem /home/conan/.conan/data/qt/5.12.4/lkeb/stable/package/e46c6e02f2760cd2e42160a54432cf1e7354d4ec/include/QtPrintSupport  -O3 -DNDEBUG -fPIC   -fPIC -std=c++17 -o CMakeFiles/HDPS_Private.dir/src/Core.cpp.o -c /home/conan/.conan/data/hdps-core/0.1.0/lkeb/stable/build/05b936f4a20297a72984f1cfeff9f028e5d42576/core/HDPS/src/Core.cpp

In file included from /usr/include/c++/9/pstl/parallel_backend.h:14,

                 from /usr/include/c++/9/pstl/algorithm_impl.h:25,

                 from /usr/include/c++/9/pstl/glue_execution_defs.h:52,

                 from /usr/include/c++/9/execution:32,

                 from /home/conan/.conan/data/hdps-core/0.1.0/lkeb/stable/build/05b936f4a20297a72984f1cfeff9f028e5d42576/core/HDPS/src/Core.cpp:27:

/usr/include/c++/9/pstl/parallel_backend_tbb.h:19:10: fatal error: tbb/blocked_range.h: No such file or directory

   19 | #include <tbb/blocked_range.h>

*/
#include <execution> // TODO Just testing C++17 support
#endif

namespace hdps
{

Core::Core(gui::MainWindow& mainWindow)
: _mainWindow(mainWindow)
{
    
}

Core::~Core()
{
    // Delete the plugin manager
    _pluginManager.reset();

    destroyPlugins();
}

void Core::init()
{
    _pluginManager = std::make_unique<plugin::PluginManager>(*this);
    _dataManager = std::make_unique<DataManager>();

    _pluginManager->loadPlugins();
}

void Core::addPlugin(plugin::Plugin* plugin)
{
    plugin->setCore(this);

    switch (plugin->getType())
    {
        // If the plugin is RawData, then add it to the data manager
        case plugin::Type::DATA: _dataManager->addRawData(dynamic_cast<plugin::RawData*>(plugin)); break;

        // If it is a view plugin then it should be added to the main window
        case plugin::Type::VIEW: _mainWindow.addView(dynamic_cast<plugin::ViewPlugin*>(plugin)); // fallthrough

        // Otherwise add the plugin to a list of plug-ins of the same type
        default: _plugins[plugin->getType()].push_back(std::unique_ptr<plugin::Plugin>(plugin));
    }
    
    // Initialize the plugin after it has been added to the core
    plugin->init();

    // If it is an analysis plugin with a settings widget, add the settings to the main window
    if (plugin->getType() == plugin::Type::ANALYSIS)
    {
        plugin::AnalysisPlugin* analysis = dynamic_cast<plugin::AnalysisPlugin*>(plugin);
        if (analysis->hasSettings())
        {
            _mainWindow.addSettings(analysis->getSettings());
        }
    }
    // If it is a loader plugin it should call loadData
    if (plugin->getType() == plugin::Type::LOADER)
    {
        try
        {
            dynamic_cast<plugin::LoaderPlugin*>(plugin)->loadData();
        }
        catch (plugin::DataLoadException e)
        {
            QMessageBox messageBox;
            messageBox.critical(0, "Error", e.what());
            messageBox.setFixedSize(500, 200);
        }
    }
    // If it is a writer plugin it should call writeData
    if (plugin->getType() == plugin::Type::WRITER)
    {
        dynamic_cast<plugin::WriterPlugin*>(plugin)->writeData();
    }
    // If the plugin is a data consumer, notify it about all the data present in the core
    plugin::DataConsumer* dataConsumer = dynamic_cast<plugin::DataConsumer*>(plugin);
    if (dataConsumer)
    {
        for (const auto& pair : _dataManager->allSets())
        {
            const Set& set = *pair.second;
            if (supportsSet(dataConsumer, set.getName()))
                dataConsumer->dataAdded(set.getName());
        }
    }
}

const QString Core::addData(const QString kind, const QString nameRequest)
{
    // Create a new plugin of the given kind
    QString rawDataName = _pluginManager->createPlugin(kind);
    // Request it from the core
    const plugin::RawData& rawData = requestData(rawDataName);

    // Create an initial full set and an empty selection belonging to the raw data
    Set* fullSet = rawData.createSet();
    Set* selection = rawData.createSet();

    // Set the properties of the new sets
    fullSet->setAll();

    // Add them to the data manager
    QString setName = _dataManager->addSet(nameRequest, fullSet);
    _dataManager->addSelection(rawDataName, selection);
    
    return setName;
}

void Core::removeData(const QString dataName)
{
    QStringList removedSets = _dataManager->removeRawData(dataName);

    for (const QString& name : removedSets)
    {
        notifyDataRemoved(name);
    }
}

const QString Core::createDerivedData(const QString kind, const QString nameRequest, const QString sourceName)
{
    // Create a new plugin of the given kind
    QString pluginName = _pluginManager->createPlugin(kind);
    // Request it from the core
    plugin::RawData& rawData = requestData(pluginName);

    rawData.setDerived(true, sourceName);

    // Create an initial full set, but no selection because it is shared with the source data
    Set* fullSet = rawData.createSet();

    // Set properties of the new set
    fullSet->setAll();
    
    // Add them to the data manager
    QString setName = _dataManager->addSet(nameRequest, fullSet);

    return setName;
}

void Core::createSubsetFromSelection(const Set& selection, const QString dataName, const QString nameRequest)
{
    // Create a new set with only the indices that were part of the selection set
    Set* newSet = selection.copy();

    newSet->_dataName = dataName;

    // Add the set the core and publish the name of the set to all plug-ins
    QString setName = _dataManager->addSet(nameRequest, newSet);
    notifyDataAdded(setName);
}

plugin::RawData& Core::requestData(const QString name)
{
    try
    {
        return _dataManager->getRawData(name);
    }
    catch (DataNotFoundException e)
    {
        QMessageBox::critical(nullptr, QString("HDPS"), e.what(), QMessageBox::Ok);
    }
}

Set& Core::requestSet(const QString name)
{
    try
    {
        return _dataManager->getSet(name);
    } 
    catch (SetNotFoundException e)
    {
        QMessageBox::critical(nullptr, QString("HDPS"), e.what(), QMessageBox::Ok);
    }
}

Set& Core::requestSelection(const QString name)
{
    try
    {
        return _dataManager->getSelection(name);
    }
    catch (const SelectionNotFoundException e)
    {
        QMessageBox::critical(nullptr, QString("HDPS"), e.what(), QMessageBox::Ok);
    }
}

/**
 * Goes through all plug-ins stored in the core and calls the 'dataAdded' function
 * on all plug-ins that inherit from the DataConsumer interface.
 */
void Core::notifyDataAdded(const QString name)
{
    for (plugin::DataConsumer* dataConsumer : getDataConsumers())
    {
        if (supportsSet(dataConsumer, name))
            dataConsumer->dataAdded(name);
    }
}

/**
* Goes through all plug-ins stored in the core and calls the 'dataChanged' function
* on all plug-ins that inherit from the DataConsumer interface.
*/
void Core::notifyDataChanged(const QString name)
{
    for (plugin::DataConsumer* dataConsumer : getDataConsumers())
    {
        if (supportsSet(dataConsumer, name))
            dataConsumer->dataChanged(name);
    }
}

/**
* Goes through all plug-ins stored in the core and calls the 'dataRemoved' function
* on all plug-ins that inherit from the DataConsumer interface.
*/
void Core::notifyDataRemoved(const QString name)
{
    for (plugin::DataConsumer* dataConsumer : getDataConsumers())
    {
        dataConsumer->dataRemoved(name);
    }
}

/** Notify all data consumers that a selection has changed. */
void Core::notifySelectionChanged(const QString dataName)
{
    for (plugin::DataConsumer* dataConsumer : getDataConsumers())
    {
        dataConsumer->selectionChanged(dataName);
    }
}

gui::MainWindow& Core::gui() const {
    return _mainWindow;
}

/** Checks if the given data consumer supports the kind data in the given set. */
bool Core::supportsSet(plugin::DataConsumer* dataConsumer, QString setName)
{
    const hdps::Set& set = requestSet(setName);
    const plugin::RawData& rawData = requestData(set.getDataName());

    return dataConsumer->supportedDataKinds().contains(rawData.getKind());
}

/** Retrieves all data consumers from the plug-in list. */
std::vector<plugin::DataConsumer*> Core::getDataConsumers()
{
    std::vector<plugin::DataConsumer*> dataConsumers;
    for (auto& kv : _plugins)
    {
        for (auto&& plugin: kv.second)
        {
            plugin::DataConsumer* dc = dynamic_cast<plugin::DataConsumer*>(plugin.get());

            if (dc)
                dataConsumers.push_back(dc);
        }
    }
    return dataConsumers;
}

/** Destroys all plug-ins kept by the core */
void Core::destroyPlugins()
{
    for (auto& kv : _plugins)
    {
        for (int i = 0; i < kv.second.size(); ++i)
        {
            kv.second[i].reset();
        }
    }
}

} // namespace hdps
