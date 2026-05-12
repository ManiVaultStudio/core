// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <util/WorkflowContextBase.h>

#include <QVariant>

class PointData;

/**
 *
 * @author T. Kroes
 */
class DimensionsSerializer : public QObject
{
public:

    static constexpr quint32 FormatVersion = 2;

    static void fromVariantMap(const QVariantMap& pointsMap, PointData* pointData);

    static QVariantMap toVariantMap(const std::vector<QString>& dimensionNames);

private:

    struct DimensionsLoadContext : public WorkflowContextBase
    {
        explicit DimensionsLoadContext(QVariantMap map) :
            _map(std::move(map))
        {
        }

        QVariantMap _map;
        QStringList _dimensionNames;
    };
};
