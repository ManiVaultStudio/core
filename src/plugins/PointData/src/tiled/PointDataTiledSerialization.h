#pragma once

#include <QVariantMap>

#include <cstddef>
#include <cstdint>
#include <memory>

#include <workflow/WorkflowPlan.h>

namespace mv::pointdata::tiled
{
class PointDataTiledStorage;

/**
 * @brief Serializes and restores the indexed tiled point-data representation.
 *
 * Tiled serialization keeps geometry and tile descriptors in the project
 * metadata while storing each tile payload through the existing blob
 * infrastructure. This separation allows deserialization to restore the
 * complete tile index immediately and defer decoding until a tile is
 * requested. The versioned representation is additive and does not replace
 * the legacy eager point-data format.
 *
 * @author Thomas Kroes
 */
class PointDataTiledSerialization
{
public:
    /**
     * Converts tiled storage metadata and payloads to a project variant map.
     *
     * Each tile is represented by independent blob metadata so project
     * loading can retain the tile index without decoding the complete matrix.
     */
    static QVariantMap toVariantMap(const PointDataTiledStorage& storage);

    /**
     * Creates a workflow that encodes tiles in parallel and publishes their
     * metadata in descriptor order.
     *
     * Tile encoding is independent because every tile owns its blob metadata.
     * The final publication stage preserves the deterministic order used by
     * the synchronous serializer and therefore keeps the serialized format
     * compatible.
     */
    static mv::workflow::UniqueWorkflowPlan toVariantMapWorkflow(const PointDataTiledStorage& storage);

    /**
     * Reconstructs tiled storage while retaining serialized tile payloads for
     * lazy loading. The map is additive to the legacy eager point-data format;
     * callers provide the matrix geometry and element size from the enclosing
     * point-data metadata.
     */
    static std::shared_ptr<PointDataTiledStorage> fromVariantMap(const QVariantMap& map, std::uint64_t rows, std::uint64_t columns, std::size_t elementSize);
};
}
