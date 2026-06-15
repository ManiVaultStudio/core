// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include <QVariantMap>

class PointData;
class Points;

namespace mv::legacy
{

/** Utility class for managing legacy serialization of point data */
class PointDataLegacySerializer final
{
public:

    /**
	 * Deserializes point data from a variant map in the format used by ManiVault versions prior to 1.5.0.
	 * @param pointData The point data object to populate with the deserialized data
	 * @param variantMap A QVariantMap containing the serialized point data in the legacy format
	 */
    static void fromVariantMapPre150(PointData& pointData, const QVariantMap& variantMap);
};

/** Utility class for managing legacy serialization of points */
class PointsLegacySerializer final
{
public:

    /**
     * Deserializes points from a variant map in the format used by ManiVault versions prior to 1.5.0.
     * @param points The points object to populate with the deserialized data
     * @param variantMap A QVariantMap containing the serialized points in the legacy format
     */
    static void fromVariantMapPre150(Points& points, const QVariantMap& variantMap);
};

}