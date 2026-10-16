// SPDX-License-Identifier: LGPL-3.0-or-later

#include "PackedTileFile.h"

#include <QDataStream>

#include <stdexcept>

namespace mv::pointdata::tiled
{
namespace
{

constexpr char packMagic[] = "MVPTPK01";
constexpr qsizetype packHeaderSize = sizeof(packMagic) - 1;
constexpr qsizetype blockHeaderSize = sizeof(std::uint64_t) * 2;

void writePackHeader(QFile& file)
{
    if (file.write(packMagic, packHeaderSize) != packHeaderSize)
        throw std::runtime_error("Unable to write packed tile file header");
}

void validatePackHeader(QFile& file)
{
    if (!file.seek(0))
        throw std::runtime_error("Unable to seek to packed tile file header");

    const auto magic = file.read(packHeaderSize);

    if (magic != QByteArray(packMagic, packHeaderSize))
        throw std::runtime_error("Invalid packed tile file header");
}

}

PackedTileFileWriter::PackedTileFileWriter(const QString& filePath, const mv::util::SharedCodec& codec) :
    _file(filePath),
    _codec(codec)
{
    if (!_codec)
        throw std::invalid_argument("Packed tile file writer requires a codec");

    if (!_file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        throw std::runtime_error("Unable to open packed tile file for writing");

    writePackHeader(_file);
}

PackedTileBlock PackedTileFileWriter::append(const char* data, const std::size_t size)
{
    if (_closed)
        throw std::logic_error("Cannot append to a closed packed tile file");

    if (data == nullptr || size == 0)
        throw std::invalid_argument("Packed tile file block is empty");

    const auto encoded = _codec->encode(data, static_cast<qsizetype>(size));
    const auto blockHeaderOffset = static_cast<std::uint64_t>(_file.pos());
    QDataStream stream(&_file);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<quint64>(size) << static_cast<quint64>(encoded.size());

    if (stream.status() != QDataStream::Ok)
        throw std::runtime_error("Unable to write packed tile block header");

    const auto payloadOffset = blockHeaderOffset + blockHeaderSize;

    if (_file.write(encoded) != encoded.size())
        throw std::runtime_error("Unable to write packed tile block payload");

    return { payloadOffset, static_cast<std::uint64_t>(encoded.size()), static_cast<std::uint64_t>(size) };
}

PackedTileBlock PackedTileFileWriter::append(const QByteArray& data)
{
    return append(data.constData(), static_cast<std::size_t>(data.size()));
}

void PackedTileFileWriter::close()
{
    if (_closed)
        return;

    if (!_file.flush())
        throw std::runtime_error("Unable to flush packed tile file");

    _file.close();
    _closed = true;
}

PackedTileFileReader::PackedTileFileReader(const QString& filePath, const mv::util::SharedCodec& codec) :
    _filePath(filePath),
    _codec(codec)
{
    if (!_codec)
        throw std::invalid_argument("Packed tile file reader requires a codec");

    QFile file(_filePath);

    if (!file.open(QIODevice::ReadOnly))
        throw std::runtime_error("Unable to open packed tile file for reading");

    validatePackHeader(file);
}

QByteArray PackedTileFileReader::read(const PackedTileBlock& block) const
{
    QByteArray result(static_cast<qsizetype>(block.uncompressedSize), '\0');
    readTo(block, result.data(), static_cast<std::size_t>(result.size()));
    return result;
}

void PackedTileFileReader::readTo(const PackedTileBlock& block, char* destination, const std::size_t destinationSize) const
{
    if (block.compressedSize == 0 || block.uncompressedSize == 0)
        throw std::invalid_argument("Packed tile file block is empty");

    _codec->decodeFromFileRangeTo(_filePath, block.offset, block.compressedSize, destination, destinationSize);
}

}
