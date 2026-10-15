// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "util/BlobCodec.h"

/**
 * @brief Encodes blob data with Zstandard compression.
 *
 * ZstdBlobCodec compresses and decompresses raw blob data using the compression
 * level supplied by its codec settings action.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class ZstdBlobCodec final : public mv::util::BlobCodec
{
public:

    /**
     * @brief Constructs a Zstandard blob codec.
     * @param parent Optional parent object.
     * @param codecSettingsAction Codec settings action used by this codec.
     */
    explicit ZstdBlobCodec(QObject* parent, mv::gui::CodecSettingsAction* codecSettingsAction);

    /**
     * @brief Returns the codec type.
     * @return Blob codec type.
     */
    [[nodiscard]] Type getType() const override;

    /**
     * @brief Returns the codec name.
     * @return Name used for serialization and diagnostics.
     */
    [[nodiscard]] QString getName() const override;

    /**
     * @brief Encodes raw bytes.
     * @param input Raw input bytes.
     * @return Encoded bytes.
     */
    [[nodiscard]] QByteArray encode(const QByteArray& input) const override;

    /**
     * @brief Encodes raw bytes.
     * @param data Pointer to raw input bytes.
     * @param size Size of the raw input in bytes.
     * @return Encoded bytes.
     */
    [[nodiscard]] QByteArray encode(const char* data, qsizetype size) const override;

    /**
     * @brief Decodes encoded bytes.
     * @param input Encoded input bytes.
     * @param expectedSize Expected decoded size in bytes, or -1 if unknown.
     * @return Decoded bytes.
     */
    [[nodiscard]] QByteArray decode(const QByteArray& input, qsizetype expectedSize = -1) const override;

    /**
     * @brief Decodes encoded bytes into an output buffer.
     * @param encodedData Zstandard-encoded input bytes.
     * @param destination Output buffer.
     * @param destinationSize Size of the output buffer in bytes.
     */
    void decodeTo(const QByteArray& encodedData, char* destination, std::uint64_t destinationSize) const override;

    /**
     * @brief Decodes data from a file.
     * @param filePath Source file path.
     * @param expectedSize Expected decoded size in bytes, or -1 if unknown.
     * @return Decoded bytes.
     */
    [[nodiscard]] QByteArray decodeFromFile(const QString& filePath, qsizetype expectedSize = -1) const override;

    /**
     * @brief Decodes file data into an output buffer.
     * @param filePath Source file path.
     * @param destination Output buffer.
     * @param destinationSize Size of the output buffer in bytes.
     */
    void decodeFromFileTo(const QString& filePath, char* destination, std::uint64_t destinationSize) const override;

    /**
     * @brief Returns the file extension used by this codec.
     * @return File extension without a leading dot.
     */
    [[nodiscard]] QString getFileExtension() const override;
};
