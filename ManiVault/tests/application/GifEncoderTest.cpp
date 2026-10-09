#include "GifEncoder.h"

#include <QDebug>

namespace {
bool require(bool condition, const char* message)
{
    if (!condition)
        qCritical().noquote() << message;
    return condition;
}
}

namespace {
QImage frame(const QSize& size, const QColor& color)
{
    QImage image(size, QImage::Format_RGBA8888);
    image.fill(color);
    return image;
}
}

bool rejectsInvalidDimensionsAndEmptyInput()
{
    GifEncoder encoder;
    QString error;
    return require(!encoder.begin({}, error), "invalid dimensions were accepted")
        && require(encoder.finish(error).isEmpty(), "empty encoder produced output");
}

bool rejectsOversizedAndDuplicateBegins()
{
    GifEncoder encoder;
    QString error;
    return require(!encoder.begin(QSize(65536, 1), error), "oversized dimensions were accepted")
        && require(encoder.begin(QSize(8, 8), error), error.toLocal8Bit().constData())
        && require(!encoder.begin(QSize(8, 8), error), "duplicate encoder start was accepted")
        && require(encoder.finish(error).isEmpty(), "encoder without frames produced output");
}

bool producesAnimatedGifFromSyntheticFrames()
{
    GifEncoder encoder;
    QString error;
    if (!require(encoder.begin(QSize(16, 16), error), error.toLocal8Bit().constData()))
        return false;
    if (!require(encoder.addFrame(frame(QSize(16, 16), Qt::red), 12, error), error.toLocal8Bit().constData()))
        return false;
    if (!require(encoder.addFrame(frame(QSize(16, 16), Qt::blue), 13, error), error.toLocal8Bit().constData()))
        return false;

    const auto data = encoder.finish(error);
    return require(!data.isEmpty(), error.toLocal8Bit().constData())
        && require(data.left(6) == QByteArrayLiteral("GIF89a"), "output does not have a GIF signature")
        && require(data.count(QByteArrayLiteral("\x2C")) > 1, "output does not contain multiple frames");
}

bool rejectsFramesWithUnexpectedDimensions()
{
    GifEncoder encoder;
    QString error;
    if (!require(encoder.begin(QSize(8, 8), error), error.toLocal8Bit().constData()))
        return false;
    return require(!encoder.addFrame(frame(QSize(4, 4), Qt::green), 12, error), "unexpected frame dimensions were accepted")
        && require(encoder.finish(error).isEmpty(), "failed encoder produced output");
}

bool rejectsInvalidFramesAndResetsAfterEmptyFinish()
{
    GifEncoder encoder;
    QString error;
    if (!require(encoder.begin(QSize(8, 8), error), error.toLocal8Bit().constData()))
        return false;

    const auto wrongFormat = QImage(QSize(8, 8), QImage::Format_RGB32);
    const auto validFrame = frame(QSize(8, 8), Qt::yellow);

    return require(!encoder.addFrame(wrongFormat, 12, error), "non-RGBA frame was accepted")
        && require(encoder.frameCount() == 0, "invalid frame changed the frame count")
        && require(!encoder.addFrame(validFrame, 0, error), "zero-duration frame was accepted")
        && require(encoder.frameCount() == 0, "invalid duration changed the frame count")
        && require(encoder.finish(error).isEmpty(), "encoder without valid frames produced output")
        && require(encoder.begin(QSize(8, 8), error), "encoder could not be reused after empty finish");
}

int main()
{
    return rejectsInvalidDimensionsAndEmptyInput()
        && rejectsOversizedAndDuplicateBegins()
        && producesAnimatedGifFromSyntheticFrames()
        && rejectsFramesWithUnexpectedDimensions()
        && rejectsInvalidFramesAndResetsAfterEmptyFinish() ? 0 : 1;
}
