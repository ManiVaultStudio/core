// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

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
    Image(const QSize& size, const std::uint32_t& noComponents, const QString& imageFilePath);

    std::uint16_t* data();
    QSize size() const;
    void setSize(const QSize& size);
    std::uint32_t width() const;
    std::uint32_t height() const;
    std::uint32_t noComponents() const;
    QString imageFilePath() const;
    QString dimensionName() const;
    void setDimensionName(const QString& dimensionName);

    std::uint32_t noPixels() const;
    std::uint32_t noElements() const;
    std::uint32_t pixelIndex(const std::uint32_t& x, const std::uint32_t& y) const;

    void getPixel(const std::uint32_t& x, const std::uint32_t& y, std::uint16_t* pixel) const;
    void setPixel(const std::uint32_t& x, const std::uint32_t& y, const std::uint16_t* pixel);

    void toFloatVector(std::vector<float>& pixels) const;

private:
    std::vector<std::uint16_t>    _data;
    QSize                       _size;
    std::uint32_t                _noComponents;
    QString                        _imageFilePath;
    QString                        _dimensionName;
};

inline QDebug operator<<(QDebug dbg, Image& image)
{
    dbg << QString("Image %1x%2, %3 components").arg(QString::number(image.width()), QString::number(image.height()), QString::number(image.noComponents()));

    return dbg;
}
