#pragma once

#include <vector>

#include <QString>

namespace common {

inline QStringList getDimensionNamesStringList(const std::uint32_t& numberOfDimensions, const std::vector<QString>& dimensionNames /*= std::vector<QString>()*/)
{
    QStringList dimensionNamesStringList;

    dimensionNamesStringList.reserve(static_cast<int>(numberOfDimensions));

    if (numberOfDimensions == dimensionNames.size()) {
        for (const auto& name : dimensionNames)
            dimensionNamesStringList.append(name);
    }
    else {
        for (unsigned int i = 0; i < numberOfDimensions; ++i)
            dimensionNamesStringList.append(QString::fromLatin1("Dim %1").arg(i));
    }

    return dimensionNamesStringList;
}

}