#pragma once

#include "PointDataTile.h"

#include <list>
#include <memory>
#include <unordered_map>

namespace mv::pointdata::tiled
{
class PointDataTileCache
{
public:
    explicit PointDataTileCache(std::size_t capacity = 8);
    std::shared_ptr<QByteArray> find(std::size_t key);
    void insert(std::size_t key, std::shared_ptr<QByteArray> value);
    void setCapacity(std::size_t capacity);
    std::size_t capacity() const { return _capacity; }
    std::size_t size() const { return _entries.size(); }

private:
    using Entry = std::pair<std::shared_ptr<QByteArray>, std::list<std::size_t>::iterator>;
    void trim();
    std::size_t _capacity;
    std::list<std::size_t> _lru;
    std::unordered_map<std::size_t, Entry> _entries;
};
}
