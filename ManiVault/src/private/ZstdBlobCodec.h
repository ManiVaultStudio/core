// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "util/BlobCodec.h"

class ZstdBlobCodec final : public mv::util::BlobCodec
{
public:

    /*
     * Constructs a ZstdBlobCodec with an optional codec settings action
     * @param codecSettingsAction Codec settings action for this codec
     */
    explicit ZstdBlobCodec(const mv::gui::CodecSettingsAction* codecSettingsAction = nullptr);

    [[nodiscard]] Type getType() const override;
    [[nodiscard]] QString getName() const override;

    [[nodiscard]] Result encode(const QByteArray& input) const override;
    [[nodiscard]] Result decode(const QByteArray& input, qsizetype expectedSize = -1) const override;

    /**
     * Decode a previously encoded block of zstd bytes directly to a provided output buffer.
     *
     * @param encodedData Zstd-encoded input bytes
     * @param destination Output buffer to which the decoded data is copied
     * @param destinationSize Size of the output buffer in bytes
     * @return Decoded bytes or an error
     */
    [[nodiscard]] Result decodeTo(const QByteArray& encodedData, char* destination, std::uint64_t destinationSize) const override;

    /*
     * Load encoded data from a file on disk and decode it.
     *
     * @param filePath Path of the file on disk from which the encoded data is loaded
     * @param expectedSize Expected decoded size in bytes, or -1 if unknown
     * @return Decoded bytes or an error
     */
    [[nodiscard]] Result decodeFromFile(const QString& filePath, qsizetype expectedSize = -1) const override;

    /*
     * Load encoded data from a file on disk and decode it directly to a provided output buffer.
     *
     * @param filePath Path of the file on disk from which the encoded data is loaded
     * @param destination Output buffer to which the decoded data is copied
     * @param destinationSize Size of the output buffer in bytes
     * @return Decoded bytes or an error
     */
    [[nodiscard]] Result decodeFromFileTo(const QString& filePath, char* destination, std::uint64_t destinationSize) const override;

    [[nodiscard]] QString getFileExtension() const override;

    void setCompressionLevel(int compressionLevel);
    [[nodiscard]] int getCompressionLevel() const;

private:
    int _compressionLevel;
};
