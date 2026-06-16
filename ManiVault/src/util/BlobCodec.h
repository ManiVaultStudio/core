// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include "actions/CodecSettingsAction.h"

#include <QByteArray>
#include <QString>

namespace mv::gui
{
    class CodecSettingsAction;
}

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
class CORE_EXPORT BlobCodec : public QObject
{
public:

    /** Blob codec type */
    enum class Type
    {
        None,           /** No compression */
        QtCompress,     /** Qt compression */
        Zstd,           /** Zstandard compression */

        Count
    };

public:

    /**
     * Constructs a codec with a codec settings action.
     * @param parent Pointer to parent object
     * @param codecSettingsAction Codec settings action for this codec (must be a valid pointer)
     */
    BlobCodec(QObject* parent, const gui::CodecSettingsActionPtr& codecSettingsAction);

    /* Virtual destructor for proper cleanup in derived classes */
	virtual ~BlobCodec() override = default;

    /** Returns the codec type */
    [[nodiscard]] virtual Type getType() const = 0;

    /** Returns the codec name used for serialization/debugging */
    [[nodiscard]] virtual QString getName() const = 0;

    /**
     * @brief Encode a block of raw bytes and return the encoded data.
     * @note This method will throw an exception if encoding fails. The returned QByteArray is only valid if the encoding was successful.
     * @param input Raw input bytes
     * @return Encoded bytes
     */
    [[nodiscard]] virtual QByteArray encode(const QByteArray& input) const = 0;

    /**
     * @brief Encode a block of raw bytes and return the encoded data.
     * @note This method will throw an exception if encoding fails. The returned QByteArray is only valid if the encoding was successful.
     * @param data Pointer to raw input bytes
     * @param size Size of the raw input bytes in bytes
     * @return Encoded bytes
     */
    [[nodiscard]] virtual QByteArray encode(const char* data, qsizetype size) const = 0;

    /**
     * @brief Decode a previously encoded block of bytes and return the decoded data.
     * @note This method will throw an exception if decoding fails. The returned QByteArray is only valid if the decoding was successful.
     * @param input Encoded input bytes
     * @param expectedSize Expected decoded size in bytes, or -1 if unknown
     * @return Decoded bytes
     */
    [[nodiscard]] virtual QByteArray decode(const QByteArray& input, qsizetype expectedSize = -1) const = 0;

    /**
     * @brief Decode a previously encoded block of bytes directly to a provided output buffer.
	 *
	 * The default implementation may allocate a temporary decoded buffer and copy it
	 * into the destination. Codecs should override this method when they can decode
	 * directly into the destination buffer.
	 *
	 * @param encodedData Encoded input bytes.
	 * @param destination Output buffer that receives the decoded bytes.
	 * @param destinationSize Size of the output buffer in bytes.
	 */
    virtual void decodeTo(const QByteArray& encodedData, char* destination, std::uint64_t destinationSize) const;

    /*
     * @brief Encode a block of raw bytes and save the encoded data to a file on disk.
     * @note This method will throw an exception if encoding or file writing fails.
     * @param input Raw input bytes
     * @param filePath Path of the file on disk to which the encoded data is saved
     * @param numberOfEncodedBytes Optional output parameter to receive the number of bytes that were encoded and written to the file
     */
    void encodeToFile(const QByteArray& input, const QString& filePath, std::uint64_t* numberOfEncodedBytes = nullptr) const;

    /*
     * @brief Encode a block of raw bytes and save the encoded data to a file on disk.
     * @note This method will throw an exception if encoding or file writing fails.
     * @param data Pointer to raw input bytes
     * @param size Size of the raw input bytes in bytes
     * @param filePath Path of the file on disk to which the encoded data is saved
     * @param numberOfEncodedBytes Optional output parameter to receive the number of bytes that were encoded and written to the file
     */
    void encodeToFile(const char* data, qsizetype size, const QString& filePath, std::uint64_t* numberOfEncodedBytes = nullptr) const;

    /*
     * @brief Load encoded data from a file on disk and decode it.
     * @note This method will throw an exception if file reading or decoding fails. The returned QByteArray is only valid if the decoding was successful.
     * @param filePath Path of the file on disk from which the encoded data is loaded
     * @param expectedSize Expected decoded size in bytes, or -1 if unknown
     * @return Decoded bytes
     */
    [[nodiscard]] virtual QByteArray decodeFromFile(const QString& filePath, qsizetype expectedSize = -1) const = 0;

    /*
     * @brief Load encoded data from a file on disk and decode it directly to a provided output buffer.
     * @note This method will throw an exception if file reading or decoding fails. The returned QByteArray is only valid if the decoding was successful.
     * @param filePath Path of the file on disk from which the encoded data is loaded
     * @param destination Output buffer to which the decoded data is copied
     * @param destinationSize Size of the output buffer in bytes
     */
    virtual void decodeFromFileTo(const QString& filePath, char* destination, std::uint64_t destinationSize) const = 0;

    /** Convert codec type to persistent string */
    [[nodiscard]] static QString typeToString(Type type);

    /** Convert persistent string to codec type */
    [[nodiscard]] static Type typeFromString(const QString& typeString);

    /**
     * Get file extension for this codec (without leading dot, e.g. "zstd" for Zstandard codec)
     * @return File extension for this codec
     */
    [[nodiscard]] virtual QString getFileExtension() const = 0;

    /**
     * Check if this codec supports inline data (i.e. storing encoded data directly in the database instead of as separate files on disk)
     * @return True if this codec supports inline data, false otherwise
     */
    [[nodiscard]] virtual bool supportsInlineData() const { return true; }

    /**
     * Get codec settings action for this codec (returns nullptr if no settings are needed)
     * @return Codec settings action for this codec (returns nullptr if no settings are needed)
     */
    gui::CodecSettingsAction* getSettingsAction() const;

private:
    gui::CodecSettingsActionPtr  _codecSettingsAction;    /* Cached settings action instance (if any) for this codec */
};

using SharedCodec = std::shared_ptr<BlobCodec>;

}