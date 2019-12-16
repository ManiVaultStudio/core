#pragma once

#include "Common.h"

#include <QString>
#include <QSize>
#include <QDebug>
#include <QImage>

#include <vector>

class IMAGEDATA_EXPORT Image
{
public:
    Image(const QSize& size, const std::uint32_t& noComponents, const QString& imageFilePath) :
        _data(),
        _size(),
        _noComponents(noComponents),
        _imageFilePath(imageFilePath),
        _dimensionName()
    {
        setSize(size);
    }

    std::uint16_t* data()
    {
        return _data.data();
    }

    QSize size() const
    {
        return _size;
    }

    void setSize(const QSize& size)
    {
        if (size == _size)
            return;

        _size = size;

        _data.resize(noElements());
    }

    std::uint32_t width() const
    {
        return _size.width();
    }

    std::uint32_t height() const
    {
        return _size.height();
    }

    std::uint32_t noComponents() const
    {
        return _noComponents;
    }

    std::uint32_t noPixels() const
    {
        return _size.width() * _size.height();
    }

    std::uint32_t noElements() const
    {
        return noPixels() * _noComponents;
    }

    std::uint32_t pixelIndex(const std::uint32_t& x, const std::uint32_t& y) const
    {
        return (y * _size.width()) + x;
    }

    void getPixel(const std::uint32_t& x, const std::uint32_t& y, std::uint16_t* pixel) const
    {
        const auto startPixelIndex = pixelIndex(x, y) * _noComponents;

        for (std::uint32_t c = 0; c < _noComponents; c++)
        {
            pixel[c] = static_cast<std::uint16_t>(_data[startPixelIndex + c]);
        }
    }

    void setPixel(const std::uint32_t& x, const std::uint32_t& y, const std::uint16_t* pixel)
    {
        const auto startPixelIndex = pixelIndex(x, y) * _noComponents;

        for (std::uint32_t c = 0; c < _noComponents; c++)
        {
            _data[startPixelIndex + c] = pixel[c];
        }
    }

    void toFloatVector(std::vector<float>& pixels) const
    {
        pixels.reserve(noElements());

        for (std::uint32_t i = 0; i < noElements(); i++)
        {
            pixels.push_back(static_cast<float>(_data[i]));
        }
    }

    QString imageFilePath() const
    {
        return _imageFilePath;
    }

    QString dimensionName() const {
        return _dimensionName;
    }

    void setDimensionName(const QString& dimensionName) {
        _dimensionName = dimensionName;
    }

private:
    std::vector<std::uint16_t>	_data;
    QSize						_size;
    std::uint32_t				_noComponents;
    QString						_imageFilePath;
    QString						_dimensionName;
};

inline QDebug operator<<(QDebug dbg, Image& image)
{
    dbg << QString("Image %1x%2, %3 components").arg(QString::number(image.width()), QString::number(image.height()), QString::number(image.noComponents()));

    return dbg;
}
