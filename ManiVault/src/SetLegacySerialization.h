// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include <QVariantMap>

class DatasetImpl;

namespace mv::legacy
{

/**
 * Deserializes point data from a variant map in the format used by ManiVault versions prior to 1.5.0.
 * @param dataset The dataset object to populate with the deserialized data
 * @param variantMap A QVariantMap containing the serialized point data in the legacy format
 */
void setFromVariantMapPre150(DatasetImpl& dataset, const QVariantMap& variantMap);

}