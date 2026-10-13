#pragma once

#include <QByteArray>
#include <QVariantMap>

#include <cstdint>
#include <memory>

namespace mv::pointdata::tiled
{
class PointDataTiledStorage;
struct TileDescriptor
{
    std::uint64_t rowOffset = 0;
    std::uint64_t columnOffset = 0;
    std::uint64_t rows = 0;
    std::uint64_t columns = 0;
    QVariantMap blob;
    QByteArray residentBytes;
};

class TileView
{
public:
    bool isValid() const { return static_cast<bool>(_bytes); }
    explicit operator bool() const { return isValid(); }
    const void* data() const { return _bytes ? _bytes->constData() : nullptr; }
    std::size_t sizeInBytes() const { return _bytes ? static_cast<std::size_t>(_bytes->size()) : 0; }
    std::uint64_t rowOffset() const { return _rowOffset; }
    std::uint64_t columnOffset() const { return _columnOffset; }
    std::uint64_t rows() const { return _rows; }
    std::uint64_t columns() const { return _columns; }
    std::size_t elementSize() const { return _elementSize; }
    std::shared_ptr<const void> lifetimeToken() const { return _bytes; }

private:
    friend class PointDataTiledStorage;
    std::shared_ptr<const QByteArray> _bytes;
    std::uint64_t _rowOffset = 0;
    std::uint64_t _columnOffset = 0;
    std::uint64_t _rows = 0;
    std::uint64_t _columns = 0;
    std::size_t _elementSize = 0;
};
}
