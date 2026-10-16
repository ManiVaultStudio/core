// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "pointdata_export.h"

#include <util/BlobCodec.h>

#include <QFile>
#include <QString>

#include <cstddef>
#include <cstdint>

namespace mv::pointdata::tiled
{

/**
 * @brief Describes one independently encoded block in a packed tile file.
 *
 * The offset points directly to the encoded payload. The surrounding pack file
 * may contain many blocks, but reading one descriptor only reads its own byte
 * range.
 *
 * @author T. Kroes
 */
struct PackedTileBlock
{
    std::uint64_t offset           = 0; /**< Byte offset of the encoded payload. */
    std::uint64_t compressedSize   = 0; /**< Number of encoded bytes. */
    std::uint64_t uncompressedSize = 0; /**< Number of decoded bytes. */
};

/**
 * @brief Appends independently encoded tile blocks to one pack file.
 *
 * The writer keeps the physical file count small without combining multiple
 * tiles into one compression stream. The caller remains responsible for
 * persisting the returned block descriptors in its dataset index.
 *
 * @author T. Kroes
 */
class POINTDATA_EXPORT PackedTileFileWriter
{
public:
    PackedTileFileWriter(const QString& filePath, const mv::util::SharedCodec& codec);

    /** Appends one tile payload and returns its location in the pack file. */
    PackedTileBlock append(const char* data, std::size_t size);

    /** Appends one tile payload and returns its location in the pack file. */
    PackedTileBlock append(const QByteArray& data);

    /** Flushes and closes the pack file. */
    void close();

private:
    QFile                     _file;
    mv::util::SharedCodec     _codec;
    bool                      _closed = false;
};

/**
 * @brief Reads independently encoded tile blocks from a packed tile file.
 *
 * Reading seeks to the selected block and uses the range-aware BlobCodec API.
 * It therefore does not load or decode unrelated blocks in the pack file.
 *
 * @author T. Kroes
 */
class POINTDATA_EXPORT PackedTileFileReader
{
public:
    PackedTileFileReader(const QString& filePath, const mv::util::SharedCodec& codec);

    /** Reads and decodes one packed tile block. */
    QByteArray read(const PackedTileBlock& block) const;

    /** Decodes one packed tile block directly into a caller-provided buffer. */
    void readTo(const PackedTileBlock& block, char* destination, std::size_t destinationSize) const;

private:
    QString                   _filePath;
    mv::util::SharedCodec     _codec;
};

}
