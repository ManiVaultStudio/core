// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Image.h"

Image::Image(const QSize& size, const std::uint32_t& noComponents, const QString& imageFilePath) :
    _data(),
    _size(),
    _noComponents(noComponents),
    _imageFilePath(imageFilePath),
    _dimensionName()
{
    setSize(size);
}

std::uint16_t* Image::data()
{
    return _data.data();
}

QSize Image::size() const
{
    return _size;
}

void Image::setSize(const QSize& size)
{
    if (size == _size)
        return;

    _size = size;

    _data.resize(noElements());
}

std::uint32_t Image::width() const
{
    return _size.width();
}

std::uint32_t Image::height() const
{
    return _size.height();
}

std::uint32_t Image::noComponents() const
{
    return _noComponents;
}

std::uint32_t Image::noPixels() const
{
    return _size.width() * _size.height();
}

std::uint32_t Image::noElements() const
{
    return noPixels() * _noComponents;
}

std::uint32_t Image::pixelIndex(const std::uint32_t& x, const std::uint32_t& y) const
{
    return (y * _size.width()) + x;
}

void Image::getPixel(const std::uint32_t& x, const std::uint32_t& y, std::uint16_t* pixel) const
{
    const auto startPixelIndex = pixelIndex(x, y) * _noComponents;

    for (std::uint32_t c = 0; c < _noComponents; c++)
    {
        pixel[c] = static_cast<std::uint16_t>(_data[startPixelIndex + c]);
    }
}

void Image::setPixel(const std::uint32_t& x, const std::uint32_t& y, const std::uint16_t* pixel)
{
    const auto startPixelIndex = pixelIndex(x, y) * _noComponents;

    for (std::uint32_t c = 0; c < _noComponents; c++)
    {
        _data[startPixelIndex + c] = pixel[c];
    }
}

void Image::toFloatVector(std::vector<float>& pixels) const
{
    pixels.reserve(noElements());

    for (std::uint32_t i = 0; i < noElements(); i++)
    {
        pixels.push_back(static_cast<float>(_data[i]));
    }
}

QString Image::imageFilePath() const
{
    return _imageFilePath;
}

QString Image::dimensionName() const
{
    return _dimensionName;
}

void Image::setDimensionName(const QString& dimensionName)
{
    _dimensionName = dimensionName;
}
