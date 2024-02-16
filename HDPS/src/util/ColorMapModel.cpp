// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ColorMapModel.h"

#include <QPainter>
#include <QDirIterator>

namespace mv {

namespace util {

mv::util::ColorMapModel* ColorMapModel::globalInstance = nullptr;

ColorMapModel::ColorMapModel(QObject* parent /*= nullptr*/) :
    QAbstractListModel(parent),
    _colorMaps()
{
    setupModelData();
}

int ColorMapModel::columnCount(const QModelIndex& parent) const
{
    return static_cast<int>(Column::End) + 1;
}

int ColorMapModel::rowCount(const QModelIndex& parent /* = QModelIndex() */) const
{
    return _colorMaps.count();
}

QVariant ColorMapModel::data(const QModelIndex& index, int role /* = Qt::DisplayRole */) const
{
    if (!index.isValid())
        return QVariant();

    const auto& colorMap = _colorMaps.at(index.row());

    auto icon = [](const QImage& image, const QSize& size) -> QPixmap {
        auto pixmap = QPixmap::fromImage(image).scaled(size);
        
        QPainter painter(&pixmap);

        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(QPen(QBrush(Qt::black), 1.0));
        painter.drawRect(QRect(QPoint(), size));

        return pixmap;
    };

    const auto column = static_cast<Column>(index.column());

    switch (role) {
        case Qt::DecorationRole:
        {
            switch (column) {
                case Column::Preview:
                {
                    switch (colorMap.getNoDimensions())
                    {
                        case 1:
                            return icon(colorMap.getImage(), QSize(150, 14));

                        case 2:
                            return icon(colorMap.getImage(), QSize(12, 12));

                        default:
                            break;
                    }

                    break;
                }

                case Column::Name:
                case Column::Image:
                case Column::ResourcePath:
                    break;
                }

            break;
        }

        case Qt::DisplayRole:
        {
            switch (column) {
                case Column::Preview:
                    return colorMap.getName();

                case Column::Name:
                    return colorMap.getName();

                case Column::Image:
                    break;

                case Column::ResourcePath:
                    return colorMap.getResourcePath();

                case Column::NoDimensions:
                    break;
            }

            break;
        }

        case Qt::EditRole:
        {
            switch (column) {
                case Column::Preview:
                    return QVariant();

                case Column::Name:
                    return colorMap.getName();

                case Column::Image:
                    return colorMap.getImage();

                case Column::NoDimensions:
                    return colorMap.getNoDimensions();

                case Column::ResourcePath:
                    return colorMap.getResourcePath();
            }

            break;
        }

        case Qt::TextAlignmentRole:
            return static_cast<int>(Qt::AlignLeft | Qt::AlignVCenter);

        default:
            break;
    }

    return QVariant();
}

void ColorMapModel::setupModelData()
{
    auto prefix = ":/colormaps";

    const auto noSteps = 256;

    // Create black-to-white color map
    QImage blackToWhite(noSteps, 1, QImage::Format::Format_RGB32);

    // Set black-to-white pixels
    for (int value = 0; value < noSteps; ++value)
        blackToWhite.setPixelColor(value, 0, QColor(value, value, value, 255));

    _colorMaps.append(ColorMap("Black to white", "", ColorMap::Type::OneDimensional, blackToWhite));

    // Create iterator for one-dimensional color maps
    QDirIterator iterator1D(QString("%1/1D/").arg(prefix), QDirIterator::Subdirectories);

    // Add the one-dimensional color maps
    while (iterator1D.hasNext()) {
        const auto resourcePath = iterator1D.next();
        _colorMaps.append(ColorMap(QFileInfo(resourcePath).baseName(), resourcePath, ColorMap::Type::OneDimensional, QImage(resourcePath)));
    }

    // Create iterator for two-dimensional color maps
    QDirIterator iterator2D(QString("%1/2D/").arg(prefix), QDirIterator::Subdirectories);

    // Add the two-dimensional color maps
    while (iterator2D.hasNext()) {
        const auto resourcePath = iterator2D.next();
        _colorMaps.append(ColorMap(QFileInfo(resourcePath).baseName(), resourcePath, ColorMap::Type::TwoDimensional, QImage(resourcePath)));
    }

    beginInsertRows(QModelIndex(), 0, _colorMaps.count());
    endInsertRows();
}

const ColorMap* ColorMapModel::getColorMap(const int& row) const
{
    const auto colorMapIndex = index(row, 0);

    if (!colorMapIndex.isValid())
        return nullptr;

    return &_colorMaps.at(row);
}

mv::util::ColorMapModel* ColorMapModel::getGlobalInstance()
{
    if (globalInstance == nullptr)
        globalInstance = new ColorMapModel();

    return globalInstance;
}

}
}
