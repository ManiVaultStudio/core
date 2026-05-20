// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "util/BlobCodec.h"

/**
 * BLOB Codec implementation for Z-standard compressed data.
 *
 * @author Thomas Kroes (Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class ZstdBlobCodec final : public mv::util::BlobCodec
{
public:

    /**
     * Constructs a Zstandard codec with a pointer to a parent object and a codec settings action.
     * @param parent Pointer to parent object
     * @param codecSettingsAction Codec settings action for this codec (must be a valid pointer)
     */
    explicit ZstdBlobCodec(QObject* parent, mv::gui::CodecSettingsAction* codecSettingsAction);

    /* Virtual destructor for proper cleanup */
    ~ZstdBlobCodec() override;

    /** Returns the codec type */
    [[nodiscard]] Type getType() const override;

    /** Returns the codec name used for serialization/debugging */
    [[nodiscard]] QString getName() const override;

    /**
     * @brief Encode a block of raw bytes and return the encoded data.
     * @note This method will throw an exception if encoding fails. The returned QByteArray is only valid if the encoding was successful.
     * @param input Raw input bytes
     * @return Encoded bytes
     */
    [[nodiscard]] QByteArray encode(const QByteArray& input) const override;

    /**
     * @brief Decode a previously encoded block of bytes and return the decoded data.
     * @note This method will throw an exception if decoding fails. The returned QByteArray is only valid if the decoding was successful.
     * @param input Encoded input bytes
     * @param expectedSize Expected decoded size in bytes, or -1 if unknown
     * @return Decoded bytes
     */
    [[nodiscard]] QByteArray decode(const QByteArray& input, qsizetype expectedSize = -1) const override;

    /**
     * Decode a previously encoded block of zstd bytes directly to a provided output buffer.
     *
     * @param encodedData Zstd-encoded input bytes
     * @param destination Output buffer to which the decoded data is copied
     * @param destinationSize Size of the output buffer in bytes
     * @return Decoded bytes or an error
     */
    void decodeTo(const QByteArray& encodedData, char* destination, std::uint64_t destinationSize) const override;

    /*
     * Load encoded data from a file on disk and decode it.
     *
     * @param filePath Path of the file on disk from which the encoded data is loaded
     * @param expectedSize Expected decoded size in bytes, or -1 if unknown
     * @return Decoded bytes or an error
     */
    [[nodiscard]] QByteArray decodeFromFile(const QString& filePath, qsizetype expectedSize = -1) const override;

    /*
     * Load encoded data from a file on disk and decode it directly to a provided output buffer.
     *
     * @param filePath Path of the file on disk from which the encoded data is loaded
     * @param destination Output buffer to which the decoded data is copied
     * @param destinationSize Size of the output buffer in bytes
     * @return Decoded bytes or an error
     */
    void decodeFromFileTo(const QString& filePath, char* destination, std::uint64_t destinationSize) const override;

    /**
     * Get file extension for this codec (without leading dot, e.g. "zstd" for Zstandard codec)
     * @return File extension for this codec
     */
    [[nodiscard]] QString getFileExtension() const override;
};
