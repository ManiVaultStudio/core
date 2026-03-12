// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include <QByteArray>
#include <QString>

namespace mv::util {

/**
 * Interface for encoding and decoding binary blobs.
 *
 * A blob codec transforms raw binary data into a stored representation
 * and back again. Concrete implementations may perform no compression,
 * Qt compression, Zstandard compression, etc.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT BlobCodec
{
public:

    /** Blob codec type */
    enum class Type
    {
        None,           /** No compression */
        QtCompress,     /** Qt compression */
        Zstd            /** Zstandard compression */
    };

    /** Codec operation result */
    struct Result
    {
        bool        _success = false;
        QByteArray  _data;
        QString     _error;

        [[nodiscard]] bool isSuccess() const { return _success; }
        [[nodiscard]] bool hasError() const { return !_success; }
    };

    /** Factory function for creating blob codec instances */
    using FactoryFunction = std::function<std::unique_ptr<BlobCodec>()>;

public:

    /** No need for custom constructor */
    BlobCodec() = default;

    /* Virtual destructor for proper cleanup in derived classes */
	virtual ~BlobCodec() = default;

    /** Returns the codec type */
    [[nodiscard]] virtual Type getType() const = 0;

    /** Returns the codec name used for serialization/debugging */
    [[nodiscard]] virtual QString getName() const = 0;

    /**
     * Encode a block of raw bytes.
     *
     * @param input Raw input bytes
     * @return Encoded bytes or an error
     */
    [[nodiscard]] virtual Result encode(const QByteArray& input) const = 0;

    /**
     * Decode a previously encoded block of bytes.
     *
     * @param input Encoded input bytes
     * @param expectedSize Expected decoded size in bytes, or -1 if unknown
     * @return Decoded bytes or an error
     */
    [[nodiscard]] virtual Result decode(const QByteArray& input, qsizetype expectedSize = -1) const = 0;

    /** Convert codec type to persistent string */
    [[nodiscard]] static QString typeToString(Type type);

    /** Convert persistent string to codec type */
    [[nodiscard]] static Type typeFromString(const QString& typeString);

    /**
     * Register a blob codec factory function for a given type (used for deserialization)
     * @param type Codec type
     * @param factoryFunction Factory function that creates an instance of the blob codec
     */
    static void registerCodec(Type type, FactoryFunction factoryFunction);

    /** Create a blob codec instance for a given type */
    static std::unique_ptr<BlobCodec> create(Type type);
};

}