// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StorageComponentSpec.h"

#include <QStorageInfo>
#include <QStandardPaths>

namespace mv::util
{

StorageComponentSpec::StorageComponentSpec() :
    HardwareComponentSpec("Storage"),
    _numberOfBytesAvailableInAppData(0)
{
}

bool StorageComponentSpec::meets(const HardwareComponentSpec& required) const
{
    return *this == dynamic_cast<const StorageComponentSpec&>(required) || *this > dynamic_cast<const StorageComponentSpec&>(required);
}

QStandardItem* StorageComponentSpec::getStandardItem() const
{
    auto item = HardwareComponentSpec::getStandardItem();

    item->setIcon(StyledIcon("hard-drive"));

    return item;
}

QString StorageComponentSpec::getFailureString(const HardwareComponentSpec& required) const
{
    if (meets(required))
        return {};

    const auto& requiredStorageComponentSpec = dynamic_cast<const StorageComponentSpec&>(required);

    return QString("Insufficient storage: %2 &lt; %3").arg(getNoBytesHumanReadable(getNumberOfBytesAvailableInAppData()), getNoBytesHumanReadable(requiredStorageComponentSpec.getNumberOfBytesAvailableInAppData()));
}

void StorageComponentSpec::fromSystem()
{
    QStorageInfo storage(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));

    _numberOfBytesAvailableInAppData = storage.bytesAvailable();

    setInitialized();
}

void StorageComponentSpec::fromVariantMap(const QVariantMap& variantMap)
{
    try {
        if (!variantMap.contains("storage"))
            throw std::runtime_error("Variant map does not contain 'storage' key.");

        const auto storageString = variantMap.value("storage", 0).toString();

    	_numberOfBytesAvailableInAppData = parseByteSize(storageString);

    	setInitialized();
    }
    catch (std::exception& e)
    {
        qCritical() << "Unable read storage hardware component properties from variant map:" << e.what();
    }
    catch (...)
    {
        qCritical() << "Unable read storage hardware component properties from variant map";
    }
}

}
