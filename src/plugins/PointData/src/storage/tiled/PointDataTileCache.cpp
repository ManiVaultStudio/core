#include "PointDataTileCache.h"

#include <algorithm>

namespace mv::pointdata::tiled
{
PointDataTileCache::PointDataTileCache(const std::size_t capacity, const std::size_t capacityBytes) :
    _capacity(std::max<std::size_t>(1, capacity)),
    _capacityBytes(std::max<std::size_t>(1, capacityBytes))
{
}

std::shared_ptr<QByteArray> PointDataTileCache::find(const std::size_t key)
{
    const auto it = _entries.find(key);
    if (it == _entries.end())
    {
        ++_misses;
        return {};
    }

    ++_hits;

    _lru.erase(it->second.second);
    _lru.push_front(key);
    it->second.second = _lru.begin();

    return it->second.first;
}

void PointDataTileCache::insert(const std::size_t key, std::shared_ptr<QByteArray> value)
{
    if (!value)
        return;

    if (const auto it = _entries.find(key); it != _entries.end())
    {
        _sizeInBytes -= static_cast<std::size_t>(it->second.first->size());
        _lru.erase(it->second.second);
        _entries.erase(it);
    }

    _sizeInBytes += static_cast<std::size_t>(value->size());

    _lru.push_front(key);
    _entries.emplace(key, std::make_pair(std::move(value), _lru.begin()));

    trim();
}

void PointDataTileCache::setCapacity(const std::size_t capacity)
{
    _capacity = std::max<std::size_t>(1, capacity);
    trim();
}

void PointDataTileCache::setCapacityBytes(const std::size_t capacityBytes)
{
    _capacityBytes = std::max<std::size_t>(1, capacityBytes);
    trim();
}

void PointDataTileCache::reset()
{
    _entries.clear();
    _lru.clear();
    _sizeInBytes = 0;
    _hits      = 0;
    _misses    = 0;
    _evictions = 0;
}

void PointDataTileCache::trim()
{
    while (_entries.size() > _capacity || _sizeInBytes > _capacityBytes)
    {
        const auto key = _lru.back();
        _lru.pop_back();

        const auto it = _entries.find(key);
        if (it != _entries.end())
        {
            _sizeInBytes -= static_cast<std::size_t>(it->second.first->size());
            _entries.erase(it);
        }

        ++_evictions;
    }
}
}
