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

int main()
{
    return rejectsInvalidDimensionsAndEmptyInput()
        && producesAnimatedGifFromSyntheticFrames()
        && rejectsFramesWithUnexpectedDimensions() ? 0 : 1;
}
