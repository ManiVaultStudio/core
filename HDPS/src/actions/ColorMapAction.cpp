#include "ColorMapAction.h"

#include <QPainter>
#include <QDirIterator>

namespace hdps {

namespace gui {

ColorMapAction::ColorMap::ColorMap(const QString& name /*= ""*/, const QString& resourcePath /*= ""*/, const Type& type /*= Type::OneDimensional*/, const QImage& image /*= QImage()*/) :
    _name(name),
    _resourcePath(resourcePath),
    _type(type),
    _image(image)
{
}

QString ColorMapAction::ColorMap::getName() const
{
    return _name;
}

QString ColorMapAction::ColorMap::getResourcePath() const
{
    return _resourcePath;
}

ColorMapAction::ColorMap::Type ColorMapAction::ColorMap::getType() const
{
    return _type;
}

QImage ColorMapAction::ColorMap::getImage() const
{
    return _image;
}

int ColorMapAction::ColorMap::getNoDimensions() const
{
    return static_cast<int>(_type);
}

ColorMapAction::ColorMapModel::ColorMapModel(QObject* parent, const ColorMap::Type& type) :
    QAbstractListModel(parent),
    _colorMaps()
{
    setupModelData();
}

int ColorMapAction::ColorMapModel::columnCount(const QModelIndex& parent) const
{
    return static_cast<int>(Column::End) + 1;
}

int ColorMapAction::ColorMapModel::rowCount(const QModelIndex& parent /* = QModelIndex() */) const
{
    return _colorMaps.count();
}

QVariant ColorMapAction::ColorMapModel::data(const QModelIndex& index, int role /* = Qt::DisplayRole */) const
{
    if (!index.isValid())
        return QVariant();

    const auto colorMap = _colorMaps.at(index.row());

    auto icon = [](const QImage& image, const QSize& size) {
        auto pixmap = QPixmap::fromImage(image).scaled(size);
        
        QPainter painter(&pixmap);

        painter.setPen(QPen(QBrush(QColor(30, 30, 30)), 1.5f));

        QPointF points[5] = {
            QPointF(0.0f, 0.0f),
            QPointF(size.width(), 0.0f),
            QPointF(size.width(), size.height()),
            QPointF(0.0f, size.height()),
            QPointF(0.0f, 0.0f)
        };

        painter.drawPolyline(points, 5);

        return pixmap;
    };

    const auto column = static_cast<Column>(index.column());

    switch (role) {
    case Qt::BackgroundColorRole:
    {
        return QBrush(Qt::red);
        /*QLinearGradient gradient(0, 0, 1, 1);
        gradient.setColorAt(0, QColor::fromRgb(10, 20, 100 + 10));
        gradient.setColorAt(1, QColor::fromRgb(255, 255, 255));
        return QBrush(gradient);*/
    }

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
                            return icon(colorMap.getImage(), QSize(60, 12));

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

void ColorMapAction::ColorMapModel::setupModelData()
{
    auto prefix = ":/resources/colormaps";

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

    QDirIterator iterator2D(QString("%1/2D/").arg(prefix), QDirIterator::Subdirectories);

    while (iterator2D.hasNext()) {
        const auto resourcePath = iterator2D.next();
        _colorMaps.append(ColorMap(QFileInfo(resourcePath).baseName(), resourcePath, ColorMap::Type::TwoDimensional, QImage(resourcePath)));
    }

    const auto noHueSteps = 36;
    const auto hueDelta = 360.0f / noHueSteps;

    for (int h = 0; h < noHueSteps; ++h)
    {
        const auto color = QColor::fromHsl(h * hueDelta, 255, 125);

        auto colorMapImage = QImage(32, 32, QImage::Format::Format_RGB888);

        colorMapImage.fill(color);

        const auto name = QString("[%1, %2, %3]").arg(QString::number(color.red()), QString::number(color.green()), QString::number(color.blue()));

        _colorMaps.append(ColorMap(name, "", ColorMap::Type::ZeroDimensional, colorMapImage));
    }

    beginInsertRows(QModelIndex(), 0, _colorMaps.count());
    endInsertRows();
}

const ColorMapAction::ColorMap* ColorMapAction::ColorMapModel::getColorMap(const int& row) const
{
    const auto colorMapIndex = index(row, 0);

    if (!colorMapIndex.isValid())
        return nullptr;

    return &_colorMaps.at(row);
}

ColorMapAction::ColorMapModel ColorMapAction::colorMapModel(nullptr, ColorMap::Type::OneDimensional);

ColorMapAction::ColorMapAction(QObject* parent, const QString& title /*= ""*/, const ColorMap::Type& colorMapType /*= ColorMap::Type::OneDimensional*/, const QString& colorMap /*= ""*/, const QString& defaultColorMap /*= ""*/) :
    OptionAction(parent, title, QStringList(), colorMap, defaultColorMap),
    _filteredColorMapModel(this, colorMapType)
{
    setText(title);
    initialize(colorMap, defaultColorMap);

    setCustomModel(&_filteredColorMapModel);
}

void ColorMapAction::initialize(const QString& colorMap /*= ""*/, const QString& defaultColorMap /*= ""*/)
{
    setCurrentText(colorMap);
    setDefaultText(defaultColorMap);
}

QString ColorMapAction::getColorMap() const
{
    return OptionAction::getCurrentText();
}

void ColorMapAction::setColorMap(const QString& colorMap)
{
    Q_ASSERT(!colorMap.isEmpty());

    OptionAction::setCurrentText(colorMap);

}

QString ColorMapAction::getDefaultColorMap() const
{
    return OptionAction::getDefaultText();
}

void ColorMapAction::setDefaultColorMap(const QString& defaultColorMap)
{
    Q_ASSERT(!defaultColorMap.isEmpty());

    OptionAction::setDefaultText(defaultColorMap);
}

}
}
