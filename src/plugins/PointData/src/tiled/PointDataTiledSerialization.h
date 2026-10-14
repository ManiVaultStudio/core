#pragma once

#include <QVariantMap>

#include <cstddef>
#include <cstdint>
#include <memory>

namespace mv::pointdata::tiled
{
class PointDataTiledStorage;

class PointDataTiledSerialization
{
public:
    static QVariantMap toVariantMap(const PointDataTiledStorage& storage);

    static std::shared_ptr<PointDataTiledStorage> fromVariantMap(
        const QVariantMap& map,
        std::uint64_t rows,
        std::uint64_t columns,
        std::size_t elementSize);
};
}
