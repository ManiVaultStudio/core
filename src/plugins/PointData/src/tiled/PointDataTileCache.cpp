#include "PointDataTileCache.h"

#include <algorithm>

namespace mv::pointdata::tiled
{
PointDataTileCache::PointDataTileCache(const std::size_t capacity) :
    _capacity(std::max<std::size_t>(1, capacity))
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
    _entries.erase(key);

    _lru.push_front(key);
    _entries.emplace(key, std::make_pair(std::move(value), _lru.begin()));

    trim();
}

void PointDataTileCache::setCapacity(const std::size_t capacity)
{
    _capacity = std::max<std::size_t>(1, capacity);
    trim();
}

void PointDataTileCache::trim()
{
    while (_entries.size() > _capacity) {
        const auto key = _lru.back();
        _lru.pop_back();
        _entries.erase(key);
        ++_evictions;
    }
}
}
