#include "GifRecorder.h"

#include <QCoreApplication>
#include <QFile>
#include <QEventLoop>
#include <QTemporaryDir>
#include <QTimer>

#include <functional>

namespace {
bool require(bool condition, const char* message)
{
    if (!condition)
        qCritical().noquote() << message;
    return condition;
}

QImage frame()
{
    QImage image(QSize(16, 16), QImage::Format_RGBA8888);
    image.fill(Qt::green);
    return image;
}

bool waitUntil(const std::function<bool()>& condition, int timeoutMs)
{
    if (condition())
        return true;

    QEventLoop loop;
    QTimer pollTimer;
    QTimer timeoutTimer;
    pollTimer.setInterval(5);
    QObject::connect(&pollTimer, &QTimer::timeout, &loop, [&] {
        if (condition())
            loop.quit();
    });
    QObject::connect(&timeoutTimer, &QTimer::timeout, &loop, &QEventLoop::quit);
    pollTimer.start();
    timeoutTimer.start(timeoutMs);
    loop.exec();
    return condition();
}
}

bool testIdleValidationAndSuccessfulReuse()
{
    QTemporaryDir temporaryDir;
    if (!require(temporaryDir.isValid(), "temporary directory could not be created"))
        return false;

    const auto outputPath = temporaryDir.filePath("recording.gif");
    GifRecorder recorder(nullptr, nullptr, [] { return frame(); });
    int startedCount = 0;
    int finishedCount = 0;
    QObject::connect(&recorder, &GifRecorder::recordingStarted, [&] { ++startedCount; });
    QObject::connect(&recorder, &GifRecorder::recordingFinished, [&](const QString&, int, qint64, qint64) { ++finishedCount; });

    QString error;
    return require(!recorder.start({}, error), "empty output path was accepted")
        && require(recorder.state() == GifRecorder::State::Idle, "invalid start changed the recorder state")
        && require(!recorder.stop(error), "stopping an idle recorder succeeded")
        && require(recorder.start(outputPath, error), error.toLocal8Bit().constData())
        && require(recorder.isRecording(), "recorder did not enter the recording state")
        && require(startedCount == 1, "recordingStarted was not emitted once")
        && require(!recorder.start(outputPath, error), "second start was accepted")
        && require(recorder.stop(error), error.toLocal8Bit().constData())
        && require(waitUntil([&] { return finishedCount == 1; }, 5000), "recording did not finish")
        && require(recorder.state() == GifRecorder::State::Idle, "recorder did not return to idle")
        && require(QFile::exists(outputPath), "successful recording did not create an output file")
        && require(recorder.start(outputPath, error), error.toLocal8Bit().constData())
        && require(recorder.stop(error), error.toLocal8Bit().constData())
        && require(waitUntil([&] { return finishedCount == 2; }, 5000), "recorder could not be reused");
}

bool testEmptyRecordingReturnsToIdleAndCanBeReused()
{
    QTemporaryDir temporaryDir;
    if (!require(temporaryDir.isValid(), "temporary directory could not be created"))
        return false;

    const auto outputPath = temporaryDir.filePath("recording.gif");
    int frameRequests = 0;
    GifRecorder recorder(nullptr, nullptr, [&frameRequests] {
        ++frameRequests;
        return frameRequests == 1 ? QImage{} : frame();
    });
    int failedCount = 0;
    int finishedCount = 0;
    QObject::connect(&recorder, &GifRecorder::recordingFailed, [&](const QString&) { ++failedCount; });
    QObject::connect(&recorder, &GifRecorder::recordingFinished, [&](const QString&, int, qint64, qint64) { ++finishedCount; });

    QString error;
    return require(recorder.start(outputPath, error), error.toLocal8Bit().constData())
        && require(!recorder.stop(error), "empty recording was reported as successful")
        && require(recorder.state() == GifRecorder::State::Idle, "failed recording did not return to idle")
        && require(failedCount == 1, "empty recording did not emit recordingFailed")
        && require(!QFile::exists(outputPath), "failed recording left an output file")
        && require(recorder.start(outputPath, error), error.toLocal8Bit().constData())
        && require(recorder.stop(error), error.toLocal8Bit().constData())
        && require(waitUntil([&] { return finishedCount == 1; }, 5000), "recorder could not recover after failure")
        && require(QFile::exists(outputPath), "recovered recording did not create an output file");
}

bool testMaximumDurationStopsRecording()
{
    QTemporaryDir temporaryDir;
    if (!require(temporaryDir.isValid(), "temporary directory could not be created"))
        return false;

    const auto outputPath = temporaryDir.filePath("recording.gif");
    GifRecorder recorder(nullptr, nullptr, [] { return frame(); }, 1);
    int finishedCount = 0;
    QObject::connect(&recorder, &GifRecorder::recordingFinished, [&](const QString&, int, qint64, qint64) { ++finishedCount; });

    QString error;
    return require(recorder.start(outputPath, error), error.toLocal8Bit().constData())
        && require(waitUntil([&] { return finishedCount == 1; }, 5000), "maximum duration did not stop recording")
        && require(recorder.state() == GifRecorder::State::Idle, "duration-limited recording did not return to idle")
        && require(QFile::exists(outputPath), "duration-limited recording did not create an output file");
}

int main(int argc, char* argv[])
{
    QCoreApplication application(argc, argv);
    return testIdleValidationAndSuccessfulReuse()
        && testEmptyRecordingReturnsToIdleAndCanBeReused()
        && testMaximumDurationStopsRecording() ? 0 : 1;
}
