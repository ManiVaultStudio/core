#pragma once

#include "PackedTileFile.h"

#include <QByteArray>
#include <QVariantMap>

#include <cstdint>
#include <memory>

namespace mv::pointdata::tiled
{
class PointDataTiledStorage;

/**
 * @brief Describes a rectangular point-data tile and its optional payload.
 *
 * A descriptor is the resident index entry for one tile. It records the
 * global matrix position and extent independently from the tile bytes, so a
 * tiled dataset can be opened and navigated without loading every payload.
 * The blob map is used for serialized lazy loading, while residentBytes is
 * used by dense and generated in-memory storage.
 *
 * @author Thomas Kroes
 */
struct TileDescriptor
{
    std::uint64_t rowOffset    = 0; /**< Global row offset. */
    std::uint64_t columnOffset = 0; /**< Global column offset. */
    std::uint64_t rows         = 0; /**< Number of rows in this tile. */
    std::uint64_t columns      = 0; /**< Number of columns in this tile. */
    QVariantMap blob;               /**< Serialized blob metadata and block index. */
    QByteArray residentBytes;       /**< Optional in-memory payload for eager/generated tiles. */
    PackedTileBlock packedBlock;    /**< Optional independently encoded block in a packed tile file. */
};

/**
 * @brief Read-only lease over a loaded point-data tile.
 *
 * The underlying bytes remain valid for the lifetime of the view, even if the
 * tile cache evicts the tile from its LRU index.
 *
 * @author Thomas Kroes
 */
class TileView
{
public:
    /** Returns whether the tile contains loaded bytes. */
    bool isValid() const
    {
        return static_cast<bool>(_bytes);
    }

    /** Returns whether the tile contains loaded bytes. */
    explicit operator bool() const
    {
        return isValid();
    }

    /** Returns a pointer to the tile's row-major bytes. */
    const void* data() const
    {
        return _bytes ? _bytes->constData() : nullptr;
    }

    /** Returns the size of the tile payload in bytes. */
    std::size_t sizeInBytes() const
    {
        return _bytes ? static_cast<std::size_t>(_bytes->size()) : 0;
    }

    /** Returns the global row offset of the tile. */
    std::uint64_t rowOffset() const
    {
        return _rowOffset;
    }

    /** Returns the global column offset of the tile. */
    std::uint64_t columnOffset() const
    {
        return _columnOffset;
    }

    /** Returns the number of rows in the tile. */
    std::uint64_t rows() const
    {
        return _rows;
    }

    /** Returns the number of columns in the tile. */
    std::uint64_t columns() const
    {
        return _columns;
    }

    /** Returns the size of one element in bytes. */
    std::size_t elementSize() const
    {
        return _elementSize;
    }

    /** Returns a token that keeps the tile bytes alive. */
    std::shared_ptr<const void> lifetimeToken() const
    {
        return _bytes;
    }

private:
    friend class PointDataTiledStorage;
    std::shared_ptr<const QByteArray> _bytes;       /**< Shared tile payload that provides the lease guarantee. */
    std::uint64_t                     _rowOffset    = 0; /**< Global row offset of the tile. */
    std::uint64_t                     _columnOffset = 0; /**< Global column offset of the tile. */
    std::uint64_t                     _rows         = 0; /**< Number of rows in the tile. */
    std::uint64_t                     _columns      = 0; /**< Number of columns in the tile. */
    std::size_t                       _elementSize  = 0; /**< Size of one tile element in bytes. */
};
}
