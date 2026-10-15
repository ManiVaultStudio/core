// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree

#define MSF_GIF_IMPL
#include "../../external/msf_gif/msf_gif.h"

#include "GifEncoder.h"

#include <QImage>

#include <algorithm>
#include <cstring>

struct GifEncoder::State
{
    MsfGifState gif{};
};

GifEncoder::~GifEncoder()
{
    delete _state;
}

bool GifEncoder::begin(const QSize& frameSize, QString& error)
{
    if (_state || frameSize.width() < 1 || frameSize.height() < 1 || frameSize.width() > 65535 || frameSize.height() > 65535) {
        error = QStringLiteral("GIF dimensions are invalid or the encoder has already been started.");
        return false;
    }

    auto* state = new State();
    if (!msf_gif_begin(&state->gif, frameSize.width(), frameSize.height())) {
        delete state;
        error = QStringLiteral("The GIF encoder could not allocate its state.");
        return false;
    }

    msf_gif_alpha_threshold = 0;
    msf_gif_bgra_flag = 0;
    _state = state;
    _frameSize = frameSize;
    _frameCount = 0;
    return true;
}

bool GifEncoder::addFrame(const QImage& frame, int centiseconds, QString& error)
{
    if (!_state || frame.isNull() || frame.size() != _frameSize || frame.format() != QImage::Format_RGBA8888 || centiseconds < 1) {
        error = QStringLiteral("The GIF frame is invalid or the encoder is not active.");
        return false;
    }

    auto image = frame;
    if (!msf_gif_frame(&_state->gif, image.bits(), centiseconds, 16, static_cast<int>(image.bytesPerLine()))) {
        error = QStringLiteral("The GIF encoder rejected a frame.");
        return false;
    }

    ++_frameCount;
    return true;
}

QByteArray GifEncoder::finish(QString& error)
{
    if (!_state || _frameCount == 0) {
        error = QStringLiteral("A GIF needs at least one frame.");
        delete _state;
        _state = nullptr;
        return {};
    }

    const auto result = msf_gif_end(&_state->gif);
    QByteArray data;
    if (result.data && result.dataSize > 0)
        data = QByteArray(static_cast<const char*>(result.data), static_cast<int>(result.dataSize));
    msf_gif_free(result);
    delete _state;
    _state = nullptr;

    if (data.isEmpty())
        error = QStringLiteral("The GIF encoder could not finish the recording.");
    return data;
}
