// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree

#include "GifRecorder.h"
#include "GifEncoder.h"

#include <QDateTime>
#include <QFileInfo>
#include <QFutureWatcher>
#include <QSaveFile>
#include <QScreen>
#include <QtConcurrent>
#include <QWindow>
#include <QWidget>

#include <algorithm>
#include <utility>

namespace {
// QScreen::grabWindow uses the native compositor path, which includes OpenGL-backed
// child windows on supported platforms. Minimized windows cannot be captured reliably;
// those ticks are skipped and the recording reports failure if no frame was obtained.
constexpr int FramesPerSecond = 8;
constexpr int FrameIntervalMs = 1000 / FramesPerSecond;
constexpr int MaximumWidth = 1280;
}

GifRecorder::GifRecorder(QWidget* window, QObject* parent, FrameProvider frameProvider, int maximumDurationMs) :
    QObject(parent),
    _window(window),
    _frameProvider(std::move(frameProvider)),
    _maximumDurationMs(maximumDurationMs)
{
    _timer.setInterval(FrameIntervalMs);
    connect(&_timer, &QTimer::timeout, this, &GifRecorder::captureFrame);
}

GifRecorder::~GifRecorder()
{
    _timer.stop();
    _state = State::Idle;
}

bool GifRecorder::start(const QString& outputPath, QString& error)
{
    if (_state != State::Idle) {
        error = QStringLiteral("A GIF recording is already in progress.");
        return false;
    }
    if ((!_window && !_frameProvider) || outputPath.isEmpty()) {
        error = QStringLiteral("The recording window or output path is unavailable.");
        return false;
    }

    _outputPath = outputPath;
    _frames.clear();
    _frameSize = {};
    _startTimeMs = QDateTime::currentMSecsSinceEpoch();
    _state = State::Recording;
    emit stateChanged(_state);
    captureFrame();
    if (_state == State::Recording) {
        _timer.start();
        emit recordingStarted();
    }
    return _state == State::Recording;
}

bool GifRecorder::stop(QString& error)
{
    if (_state != State::Recording) {
        error = QStringLiteral("No GIF recording is active.");
        return false;
    }

    _timer.stop();
    if (_frames.isEmpty()) {
        _state = State::Idle;
        emit stateChanged(_state);
        error = QStringLiteral("No window frames could be captured.");
        emit recordingFailed(error);
        return false;
    }

    _state = State::Encoding;
    emit stateChanged(_state);
    finishEncoding();
    return true;
}

void GifRecorder::captureFrame()
{
    if (_state != State::Recording)
        return;

    if (QDateTime::currentMSecsSinceEpoch() - _startTimeMs >= _maximumDurationMs) {
        QString ignoredError;
        [[maybe_unused]] auto result = stop(ignoredError);
        return;
    }

    QImage frame;
    if (_frameProvider) {
        frame = _frameProvider();
    }
    else {
        if (!_window || _window->isMinimized())
            return;

        auto* windowHandle = _window->windowHandle();
        auto* screen = windowHandle && windowHandle->screen() ? windowHandle->screen() : _window->screen();
        if (!screen)
            return;

        const auto pixmap = screen->grabWindow(_window->winId());
        if (pixmap.isNull())
            return;

        frame = pixmap.toImage().convertToFormat(QImage::Format_RGBA8888);
    }

    frame = normalizeFrame(frame);
    if (frame.isNull())
        return;

    if (_frameSize.isEmpty())
        _frameSize = frame.size();
    else if (frame.size() != _frameSize)
        frame = frame.scaled(_frameSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    _frames.push_back(frame);

    if (QDateTime::currentMSecsSinceEpoch() - _startTimeMs >= _maximumDurationMs) {
        QString ignoredError;
        [[maybe_unused]] auto result = stop(ignoredError);
    }
}

QImage GifRecorder::normalizeFrame(const QImage& image) const
{
    if (image.isNull())
        return {};

    const auto targetWidth = std::min(image.width(), MaximumWidth);
    const auto targetSize = image.width() > MaximumWidth ? image.size().scaled(targetWidth, image.height(), Qt::KeepAspectRatio) : image.size();
    return image.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation).convertToFormat(QImage::Format_RGBA8888);
}

void GifRecorder::finishEncoding()
{
    const auto frames = _frames;
    const auto outputPath = _outputPath;
    const auto frameSize = _frameSize;
    const auto durationMs = QDateTime::currentMSecsSinceEpoch() - _startTimeMs;
    const auto frameCount = frames.size();

    auto* watcher = new QFutureWatcher<QString>(this);
    _watcher = watcher;
    connect(watcher, &QFutureWatcher<QString>::finished, this, [this, watcher, outputPath, durationMs, frameCount] {
        const auto error = watcher->result();
        watcher->deleteLater();
        _watcher = nullptr;
        _state = State::Idle;
        emit stateChanged(_state);
        if (!error.isEmpty())
            emit recordingFailed(error);
        else
            emit recordingFinished(outputPath, frameCount, durationMs, QFileInfo(outputPath).size());
    });

    auto future = QtConcurrent::run([frames, frameSize, outputPath]() -> QString {
        GifEncoder encoder;
        QString error;
        if (!encoder.begin(frameSize, error))
            return error;
        for (int index = 0; index < frames.size(); ++index) {
            if (!encoder.addFrame(frames[index], 12 + (index % 2), error))
                return error;
        }
        const auto data = encoder.finish(error);
        if (data.isEmpty())
            return error;
        QSaveFile file(outputPath);
        if (!file.open(QIODevice::WriteOnly) || file.write(data) != data.size() || !file.commit())
            return QStringLiteral("Could not write the GIF file.");
        return {};
    });
    watcher->setFuture(future);
}
