// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ThirdPartyLicensesListModel.h"

#include "CoreInterface.h"
#include "PluginFactory.h"
#include "util/JSON.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

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

/** Load the third-party dependencies documented by ManiVault Core. */
plugin::ThirdPartyLicenses getCoreThirdPartyLicenses()
{
    QFile licenseFile(":/JSON/ThirdPartyLicenses");

    if (!licenseFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Unable to open Core third-party licenses resource" << licenseFile.fileName();
        return {};
    }

    QJsonParseError parseError;
    const auto document = QJsonDocument::fromJson(licenseFile.readAll(), &parseError);

    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        qWarning() << "Unable to parse Core third-party licenses resource:" << parseError.errorString();
        return {};
    }

    const auto licensesValue = document.object().value("thirdPartyLicenses");

    if (!licensesValue.isArray()) {
        qWarning() << "Core third-party licenses resource does not contain a thirdPartyLicenses array";
        return {};
    }

    try {
        const auto licensesJson = QJsonDocument(licensesValue.toArray()).toJson(QJsonDocument::Compact).toStdString();
        util::validateJson(licensesJson, ":/JSON/ThirdPartyLicenses", util::loadJsonFromResource(":/JSON/thirdparty.licenses.schema.json"), "https://github.com/ManiVaultStudio/core/tree/master/ManiVault/res/json/thirdparty.licenses.schema.json");
    }
    catch (const std::exception& exception) {
        qWarning() << "Core third-party licenses resource failed schema validation:" << exception.what();
        return {};
    }

    plugin::ThirdPartyLicenses licenses;

    for (const auto& licenseValue : licensesValue.toArray()) {
        if (!licenseValue.isObject()) {
            qWarning() << "Ignoring non-object Core third-party license entry";
            continue;
        }

        const auto licenseObject = licenseValue.toObject();
        const auto name = licenseObject.value("name").toString();
        const auto license = licenseObject.value("license").toString();
        const auto url = licenseObject.value("url").toString();

        if (name.isEmpty() || license.isEmpty()) {
            qWarning() << "Ignoring Core third-party license entry without a name or license";
            continue;
        }

#ifndef MV_USE_ERROR_LOGGING
        if (name == "sentry")
            continue;
#endif

        licenses.push_back({ name, license, url });
    }

    return licenses;
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
