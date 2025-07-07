// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DisplayComponentSpec.h"

namespace mv::util
{

DisplayComponentSpec::DisplayComponentSpec() :
    HardwareComponentSpec("Display Component Spec")
{
}

QString DisplayComponentSpec::getStatusString(const HardwareComponentSpecPtr& other) const
{
    if (*this < dynamic_cast<const DisplayComponentSpec&>(other))
        return "Display resolution is too low";

    return {};
}

void DisplayComponentSpec::fromSystem()
{
    if (auto screen = QGuiApplication::primaryScreen())
        _resolution = { screen->size().width(), screen->size().height() };

    setInitialized();
}

void DisplayComponentSpec::fromVariantMap(const QVariantMap& variantMap)
{
    try {
        if (!variantMap.contains("display"))
            return;

        const auto displayMap = variantMap.value("display").toMap();

        if (displayMap.contains("resolution")) {
            const auto resolutionMap = displayMap.value("resolution").toMap();

            _resolution.first   = resolutionMap.value("horizontal", 0).toInt();
            _resolution.second  = resolutionMap.value("vertical", 0).toInt();
        }

        setInitialized();
    }
    catch (std::exception& e)
    {
        qCritical() << "Unable read display hardware component properties from variant map:" << e.what();
    }
    catch (...)
    {
        qCritical() << "Unable read display hardware component properties from variant map";
    }
}

}
