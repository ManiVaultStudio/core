// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DisplayComponentSpec.h"

namespace mv::util
{

DisplayComponentSpec::DisplayComponentSpec() :
    HardwareComponentSpec("Display")
{
}

bool DisplayComponentSpec::meets(const HardwareComponentSpec& required) const
{
    return *this == dynamic_cast<const DisplayComponentSpec&>(required) || *this > dynamic_cast<const DisplayComponentSpec&>(required);
}

QString DisplayComponentSpec::getFailureString(const HardwareComponentSpec& required) const
{
    if (meets(required))
        return {};

    const auto& displayComponentSpec = dynamic_cast<const DisplayComponentSpec&>(required);

    return QString("Display resolution is too low. Required: %1x%2").arg(QString::number(displayComponentSpec._resolution.first), QString::number(displayComponentSpec._resolution.second));
}

DisplayComponentSpec::Resolution DisplayComponentSpec::getResolution() const
{
    return _resolution;
}

QStandardItem* DisplayComponentSpec::getStandardItem() const
{
    auto item = HardwareComponentSpec::getStandardItem();

    item->setIcon(StyledIcon("display"));

    auto resolutionRow = getParameterRow("Resolution");

    item->appendRow(resolutionRow);

    auto systemDisplayComponentSpec = HardwareSpec::getSystemHardwareSpec().getHardwareComponentSpec<DisplayComponentSpec>("Display");

    if (systemDisplayComponentSpec && systemDisplayComponentSpec->isInitialized()) {
        const auto systemResolution = systemDisplayComponentSpec->getResolution();

    	resolutionRow.first()->appendRow(getParameterRow("Horizontal", QString::number(systemResolution.first), QString::number(_resolution.first)));
        resolutionRow.first()->appendRow(getParameterRow("Vertical", QString::number(systemResolution.second), QString::number(_resolution.second)));
    } else {
        resolutionRow.first()->appendRow(getParameterRow("Horizontal", "Unknown"));
        resolutionRow.first()->appendRow(getParameterRow("Vertical", "Unknown"));
    }

    return item;
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
