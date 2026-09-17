// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ThirdPartyLicensesListModel.h"

#include "CoreInterface.h"
#include "PluginFactory.h"

namespace mv {

namespace {

/** Add one license occurrence to the deduplicated usage collection. */
void addUsage(ThirdPartyLicenseUsages& usages, const plugin::ThirdPartyLicense& license, bool isCore, const QString& availablePlugin, const QString& loadedPlugin)
{
    auto match = std::find_if(usages.begin(), usages.end(), [&license](const auto& usage) {
        return usage.license == license;
    });

    if (match == usages.end()) {
        usages.push_back({ license, isCore, {}, {} });
        match = std::prev(usages.end());
    }

    match->isCore = match->isCore || isCore;
    if (!availablePlugin.isEmpty() && !match->availablePlugins.contains(availablePlugin))
        match->availablePlugins.append(availablePlugin);
    if (!loadedPlugin.isEmpty() && !match->loadedPlugins.contains(loadedPlugin))
        match->loadedPlugins.append(loadedPlugin);
}

/** Return the third-party dependencies currently documented by ManiVault Core. */
plugin::ThirdPartyLicenses getCoreThirdPartyLicenses()
{
    return {
        { "Qt-Advanced-Docking-System", "LGPL-2.1", "https://github.com/githubuser0xFFFF/Qt-Advanced-Docking-System" },
        { "QuaZip", "LGPL-2.1", "https://github.com/stachenov/quazip" },
        { "zlib", "zlib", "https://zlib.net" },
        { "nlohmann json", "MIT", "https://json.nlohmann.me" },
        { "valijson", "BSD-2-Clause", "https://github.com/tristanpenman/valijson" },
        { "biovault_bfloat16", "Apache-2.0", "https://github.com/biovault/biovault_bfloat16" },
        { "Zstandard", "BSD", "https://github.com/facebook/zstd" },
        { "Taskflow", "MIT", "https://github.com/taskflow/taskflow" },
#ifdef MV_USE_ERROR_LOGGING
        { "sentry", "MIT", "https://sentry.io" },
#endif
        { "Qt", "LGPL", "https://qt.io" }
    };
}

}

ThirdPartyLicensesListModel::ThirdPartyLicensesListModel(QObject* parent) :
    AbstractThirdPartyLicensesModel(parent)
{
    connect(&mv::plugins(), &AbstractPluginManager::pluginFactoriesLoaded, this, &ThirdPartyLicensesListModel::populateFromPluginManager);
    connect(&mv::plugins(), &AbstractPluginManager::pluginAdded, this, &ThirdPartyLicensesListModel::populateFromPluginManager);
    connect(&mv::plugins(), &AbstractPluginManager::pluginAboutToBeDestroyed, this, &ThirdPartyLicensesListModel::populateFromPluginManager);

    populateFromPluginManager();
}

void ThirdPartyLicensesListModel::populateFromPluginManager()
{
    // Keep Core and plugin licenses in one collection so shared dependencies are shown once.
    ThirdPartyLicenseUsages usages;

    for (const auto& license : getCoreThirdPartyLicenses())
        addUsage(usages, license, true, {}, {});

    for (const auto* factory : mv::plugins().getPluginFactoriesByTypes()) {
        if (!factory)
            continue;

        const auto pluginName = factory->getGuiName();
        const auto loadedPlugins = mv::plugins().getPluginsByFactory(factory);

        for (const auto& license : factory->getThirdPartyLicenses()) {
            addUsage(usages, license, false, pluginName, {});
            for (const auto* plugin : loadedPlugins)
                if (plugin)
                    addUsage(usages, license, false, {}, plugin->getGuiName());
        }
    }

    setLicenseUsages(usages);
}

}
