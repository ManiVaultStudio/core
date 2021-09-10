#include "ColorMapModel.h"

#include <QPainter>
#include <QDirIterator>

namespace hdps {

namespace util {

hdps::util::ColorMapModel* ColorMapModel::globalInstance = nullptr;

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

    const auto colorMap = _colorMaps.at(index.row());

    auto icon = [](const QImage& image, const QSize& size) {
        auto pixmap = QPixmap::fromImage(image).scaled(size);
        
        QPainter painter(&pixmap);

        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::HighQualityAntialiasing);
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
                        case 0:
                            return icon(colorMap.getImage(), QSize(15, 15));

                        case 1:
                            return icon(colorMap.getImage(), QSize(150, 12));

                        case 2:
                            return icon(colorMap.getImage(), QSize(32, 32));

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
                    colorMap.getResourcePath();
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

    QImage blackToWhite(noSteps, 1, QImage::Format::Format_RGB32);

    for (int value = 0; value < noSteps; ++value) {
        blackToWhite.setPixelColor(value, 0, QColor(value, value, value, 255));
    }

    _colorMaps.append(ColorMap("Black to white", "", ColorMap::Type::OneDimensional, blackToWhite));

    QDirIterator iterator1D(QString("%1/1D/").arg(prefix), QDirIterator::Subdirectories);

    while (iterator1D.hasNext()) {
        const auto resourcePath = iterator1D.next();
        _colorMaps.append(ColorMap(QFileInfo(resourcePath).baseName(), resourcePath, ColorMap::Type::OneDimensional, QImage(resourcePath)));
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

hdps::util::ColorMapModel* ColorMapModel::getGlobalInstance()
{
    if (globalInstance == nullptr)
        globalInstance = new ColorMapModel();

    return globalInstance;
}

}
}
