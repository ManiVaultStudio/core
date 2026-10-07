// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree

#pragma once

#include <QObject>
#include <QImage>
#include <QPointer>
#include <QString>
#include <QTimer>

template<typename T> class QFutureWatcher;
class QWidget;

/**
 * @brief Developer-only recorder for the ManiVault Studio main window.
 *
 * Captures the native window through QScreen::grabWindow at 8 FPS, keeps the
 * captured frames at a maximum width of 1280 pixels, and exports them as a
 * looping animated GIF. Encoding and file writing are performed asynchronously
 * after recording stops so the GUI thread remains available for normal Studio
 * interaction.
 *
 * Minimized windows and unavailable screens cannot be captured reliably. Such
 * capture ticks are skipped; a recording with no captured frames is reported
 * as a failure. Native compositor behavior may differ between platforms and
 * window-manager configurations.
 */
class GifRecorder : public QObject
{
    Q_OBJECT

public:
    /** Current phase of the recording lifecycle. */
    enum class State {
        Idle,       /**< No recording or export is active. */
        Recording,  /**< Frames are being captured from the target window. */
        Encoding    /**< Capture has stopped and the GIF is being written. */
    };

    /**
     * Creates a recorder for the target window.
     *
     * @param window Main-window widget whose native window will be captured.
     * @param parent Optional QObject parent.
     */
    explicit GifRecorder(QWidget* window, QObject* parent = nullptr);

    /** Stops the capture timer and releases recorder-owned resources. */
    ~GifRecorder() override;

    /** @return Current recording lifecycle state. */
    State state() const { return _state; }

    /** @return True only while frames are actively being captured. */
    bool isRecording() const { return _state == State::Recording; }

    /**
     * @brief Starts capturing frames for a new recording.
     *
     * The output path is reserved for the eventual GIF and no file is created
     * until encoding completes. Starting while recording or encoding is
     * rejected. The first capture is attempted immediately, followed by the
     * fixed recording interval.
     *
     * @param outputPath Destination path for the completed GIF.
     * @param error Receives a human-readable failure description.
     * @return True when the recorder entered the Recording state.
     */
    [[nodiscard]] bool start(const QString& outputPath, QString& error);

    /**
     * @brief Stops capture and starts asynchronous GIF encoding.
     *
     * The recorder enters Encoding until recordingFinished() or
     * recordingFailed() is emitted. Calling stop() when the recorder is not
     * active, or before any frame was captured, is rejected.
     *
     * @param error Receives a human-readable failure description.
     * @return True when capture stopped and export was started.
     */
    [[nodiscard]] bool stop(QString& error);

signals:
    /** Emitted after recording has entered the Recording state. */
    void recordingStarted();

    /**
     * Emitted after the GIF was written successfully.
     *
     * @param path Output GIF path.
     * @param frameCount Number of captured frames.
     * @param durationMs Elapsed recording duration in milliseconds.
     * @param fileSize Size of the completed GIF in bytes.
     */
    void recordingFinished(const QString& path, int frameCount, qint64 durationMs, qint64 fileSize);

    /** @param error Human-readable capture, encoding, or file-writing error. */
    void recordingFailed(const QString& error);

    /** @param state New recorder lifecycle state. */
    void stateChanged(GifRecorder::State state);

private:
    /** Captures and queues one native-window frame, or stops at the duration limit. */
    void captureFrame();

    /** Starts asynchronous encoding and installation of the completed GIF. */
    void finishEncoding();

    /** Converts a native capture to the bounded RGBA frame representation. */
    QImage normalizeFrame(const QImage& image) const;

    QPointer<QWidget>           _window;                 /**< Native window capture target. */
    QTimer                      _timer;                  /**< Fixed-rate capture timer. */
    QFutureWatcher<QString>*    _watcher = nullptr;      /**< Asynchronous export watcher. */
    State                       _state = State::Idle;    /**< Current lifecycle state. */
    QString                     _outputPath;              /**< Destination path for the recording. */
    QVector<QImage>             _frames;                  /**< Captured frames awaiting encoding. */
    QSize                       _frameSize;               /**< Normalized dimensions shared by frames. */
    qint64                      _startTimeMs = 0;         /**< Wall-clock start time of the session. */
};

Q_DECLARE_METATYPE(GifRecorder::State)
