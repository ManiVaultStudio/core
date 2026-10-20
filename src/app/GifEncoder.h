// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree

#pragma once

#include <QByteArray>
#include <QImage>
#include <QString>

#include <cstdint>

/**
 * @brief Encodes a sequence of RGBA images as an animated GIF.
 *
 * This class is the ManiVault-owned boundary around the vendored `msf_gif`
 * implementation. A single instance represents one GIF encoding session. The
 * expected call sequence is begin(), one or more addFrame() calls, and then
 * finish(). The encoder owns all third-party state and releases it when the
 * session finishes or the object is destroyed.
 *
 * The encoder does not perform image conversion or rescaling. Callers must
 * provide every frame with the dimensions and QImage::Format_RGBA8888 format
 * established by begin().
 */
class GifEncoder
{
public:
    /** Creates an encoder with no active encoding session. */
    GifEncoder() = default;

    /** Releases any partially initialized encoding session. */
    ~GifEncoder();

    GifEncoder(const GifEncoder&) = delete;
    GifEncoder& operator=(const GifEncoder&) = delete;

    /**
     * @brief Starts a new encoding session.
     *
     * Starting an already active encoder is rejected. GIF dimensions are
     * limited to the positive range accepted by the GIF format and encoder.
     *
     * @param frameSize Fixed size required for every frame in this session.
     * @param error Receives a human-readable failure description.
     * @return True when the encoder state was initialized successfully.
     */
    [[nodiscard]] bool begin(const QSize& frameSize, QString& error);

    /**
     * @brief Appends one RGBA frame to the active GIF.
     *
     * The image must be non-null, use QImage::Format_RGBA8888, and have the
     * exact dimensions supplied to begin(). The frame delay uses GIF
     * centiseconds; callers should account for that format limitation when
     * selecting a frame rate.
     *
     * @param frame RGBA image to append.
     * @param centiseconds Display duration of the frame in hundredths of a
     * second.
     * @param error Receives a human-readable failure description.
     * @return True when the frame was accepted by the encoder.
     */
    [[nodiscard]] bool addFrame(const QImage& frame, int centiseconds, QString& error);

    /**
     * @brief Completes the active session and returns the encoded GIF bytes.
     *
     * The encoder state is released whether encoding succeeds or fails. An
     * empty result indicates that the session was invalid, contained no
     * frames, or could not be finalized.
     *
     * @param error Receives a human-readable failure description when the
     * result is empty.
     * @return Complete GIF file data, or an empty byte array on failure.
     */
    [[nodiscard]] QByteArray finish(QString& error);

    /** @return Whether at least one frame has been accepted in the session. */
    bool hasFrames() const { return _frameCount > 0; }

    /** @return Number of frames accepted in the active or completed session. */
    int frameCount() const { return _frameCount; }

    /** @return Fixed frame dimensions selected by begin(). */
    QSize frameSize() const { return _frameSize; }

private:
    struct State;
    State* _state = nullptr;            /**< Opaque msf_gif state for the active session. */
    QSize  _frameSize;                  /**< Fixed dimensions required for session frames. */
    int    _frameCount = 0;             /**< Number of successfully submitted frames. */
};
