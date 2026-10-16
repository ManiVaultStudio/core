#pragma once

#include "PointDataTile.h"

#include <cstdint>
#include <list>
#include <memory>
#include <unordered_map>

namespace mv::pointdata::tiled
{
/**
 * @brief Thread-unsafe LRU cache for loaded tile payloads.
 *
 * The cache owns shared payload objects rather than exposing references into
 * its containers. This is important for tiled point data: removing an entry
 * from the LRU index must not invalidate a TileView that is still being used
 * by a caller. PointDataTiledStorage serializes access to this cache through
 * its storage mutex.
 *
 * @author Thomas Kroes
 */
class PointDataTileCache
{
public:
    /** Constructs a cache with the specified maximum number of entries. */
    explicit PointDataTileCache(std::size_t capacity = 8);

    /** Finds a payload and promotes it to the most recently used position. */
    std::shared_ptr<QByteArray> find(std::size_t key);

    /** Inserts or replaces a payload and evicts old entries if necessary. */
    void insert(std::size_t key, std::shared_ptr<QByteArray> value);

    /** Changes the maximum number of cached entries. */
    void setCapacity(std::size_t capacity);

    /** Returns the maximum number of cached entries. */
    std::size_t capacity() const
    {
        return _capacity;
    }

    /** Returns the current number of cached entries. */
    std::size_t size() const
    {
        return _entries.size();
    }

    /** Returns the number of successful lookups. */
    std::uint64_t hits() const
    {
        return _hits;
    }

    /** Returns the number of unsuccessful lookups. */
    std::uint64_t misses() const
    {
        return _misses;
    }

    /** Returns the number of entries removed because of capacity limits. */
    std::uint64_t evictions() const
    {
        return _evictions;
    }

private:
    using Entry = std::pair<std::shared_ptr<QByteArray>, std::list<std::size_t>::iterator>;
    void trim();
    std::size_t                           _capacity;      /**< Maximum number of resident payloads. */
    std::list<std::size_t>                _lru;           /**< Keys ordered from least to most recently used. */
    std::unordered_map<std::size_t, Entry> _entries;      /**< Payloads and their positions in the LRU list. */
    std::uint64_t                         _hits      = 0; /**< Number of successful lookups. */
    std::uint64_t                         _misses    = 0; /**< Number of unsuccessful lookups. */
    std::uint64_t                         _evictions = 0; /**< Number of payloads removed by capacity trimming. */
};
}
