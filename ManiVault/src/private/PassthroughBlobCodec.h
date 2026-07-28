// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include <util/BlobCodec.h>

/**
 * @brief Encodes blob data without compression.
 *
 * PassthroughBlobCodec copies input data unchanged. It is useful when the
 * serialization pipeline needs to use the blob codec interface without applying
 * compression.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class PassthroughBlobCodec final : public mv::util::BlobCodec
{
public:
    
    /**
     * @brief Constructs a pass-through blob codec.
     * @param parent Optional parent object.
     * @param codecSettingsAction Codec settings action used by this codec.
     */
    explicit PassthroughBlobCodec(QObject* parent, mv::gui::CodecSettingsAction* codecSettingsAction);

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
     * @brief Encodes raw bytes to a file.
     * @param data Pointer to raw input bytes.
     * @param size Size of the raw input in bytes.
     * @param filePath Destination file path.
     * @param numberOfEncodedBytes Optional output for the number of bytes written.
     */
    void encodeToFile(const char* data, qsizetype size, const QString& filePath, std::uint64_t* numberOfEncodedBytes) const;

    /**
     * @brief Decodes encoded bytes.
     * @param input Encoded input bytes.
     * @param expectedSize Expected decoded size in bytes, or -1 if unknown.
     * @return Decoded bytes.
     */
    [[nodiscard]] QByteArray decode(const QByteArray& input, qsizetype expectedSize = -1) const override;

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
