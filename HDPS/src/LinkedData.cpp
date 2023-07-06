// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LinkedData.h"
#include "Set.h"

using namespace hdps::util;

namespace hdps
{

SelectionMap::SelectionMap(Type type /*= Indexed*/) :
    Serializable("SelectionMapping"),
    _type(type)
{
}

SelectionMap::SelectionMap(const QSize& sourceImageSize, const QSize& targetImageSize) :
    Serializable("SelectionMapping"),
    _type(Type::ImagePyramid),
    _sourceImageSize(sourceImageSize),
    _targetImageSize(targetImageSize)
{
}

void SelectionMap::populateMappingIndices(std::uint32_t pointIndex, Indices& indices) const
{
    switch (_type)
    {
        case Type::Indexed:
            indices = _map.at(pointIndex);
            break;

        case Type::ImagePyramid:
        {
            const QPoint sourcePixelPosition(pointIndex % _sourceImageSize.width(), static_cast<std::int32_t>(floorf(static_cast<float>(pointIndex) / static_cast<float>(_sourceImageSize.width()))));

            const auto scaleFactor = static_cast<float>(_targetImageSize.width()) / static_cast<float>(_sourceImageSize.width());

            if (scaleFactor == 1.0f)
                indices = Indices({ pointIndex });

            if (scaleFactor < 1.0f) {
                const auto levelFactor = static_cast<std::uint32_t>(floorf(1.0f / scaleFactor));

                const QPoint targetPixelPosition(
                    static_cast<std::int32_t>(floorf(static_cast<float>(sourcePixelPosition.x()) / static_cast<float>(levelFactor))),
                    static_cast<std::int32_t>(floorf(static_cast<float>(sourcePixelPosition.y()) / static_cast<float>(levelFactor)))
                );

                indices = Indices({ static_cast<std::uint32_t>(targetPixelPosition.y() * _targetImageSize.width() + targetPixelPosition.x()) });
            }

            if (scaleFactor > 1.0f) {
                const auto levelFactor = static_cast<std::uint32_t>(floorf(scaleFactor));

                const QPoint targetPixelPositionStart(sourcePixelPosition.x() * levelFactor, sourcePixelPosition.y() * levelFactor);
                const QPoint targetPixelPositionEnd(targetPixelPositionStart + QPoint(levelFactor, levelFactor));

                indices.clear();
                indices.reserve(static_cast<size_t>(levelFactor) * static_cast<size_t>(levelFactor));

                for (std::int32_t targetPixelX = targetPixelPositionStart.x(); targetPixelX < targetPixelPositionEnd.x(); ++targetPixelX) {
                    for (std::int32_t targetPixelY = targetPixelPositionStart.y(); targetPixelY < targetPixelPositionEnd.y(); ++targetPixelY) {
                        if (targetPixelX >= _targetImageSize.width() || targetPixelY >= _targetImageSize.height())
                            break;

                        indices.push_back(targetPixelY * _targetImageSize.width() + targetPixelX);
                    }
                }
            }

            break;
        }
    }
}

SelectionMap::Map& SelectionMap::getMap()
{
    return _map;
}

bool SelectionMap::hasMappingForPointIndex(std::uint32_t pointIndex) const
{
    switch (_type)
    {
        case Type::Indexed:
            return _map.find(pointIndex) != _map.end();

        case Type::ImagePyramid:
            return pointIndex < static_cast<std::uint32_t>(_sourceImageSize.width() * _sourceImageSize.height());
    }

    return false;
}

void SelectionMap::fromVariantMap(const QVariantMap& variantMap)
{
    variantMapMustContain(variantMap, "Type");
    variantMapMustContain(variantMap, "Indices");
    variantMapMustContain(variantMap, "IndicesSize");
    variantMapMustContain(variantMap, "Ranges");
    variantMapMustContain(variantMap, "RangesSize");
    variantMapMustContain(variantMap, "SourceImageSize");
    variantMapMustContain(variantMap, "TargetImageSize");

    _type = static_cast<Type>(variantMap["Type"].toInt());

    switch (_type)
    {
        case Type::Indexed:
        {
            std::vector<std::uint32_t> indices;
            std::vector<std::uint32_t> ranges;

            indices.resize(variantMap["IndicesSize"].toInt());
            ranges.resize(variantMap["RangesSize"].toInt());

            populateDataBufferFromVariantMap(variantMap["Indices"].toMap(), (char*)indices.data());
            populateDataBufferFromVariantMap(variantMap["Ranges"].toMap(), (char*)ranges.data());

            for (int i = 0; i < ranges.size(); i += 3)
                _map[ranges[i]] = std::vector<std::uint32_t>(indices.begin() + ranges[i + 1], indices.begin() + ranges[i + 2]);

            break;
        }

        case Type::ImagePyramid:
        {
            _sourceImageSize.setWidth(variantMap["SourceImageSize"].toMap()["Width"].toInt());
            _sourceImageSize.setHeight(variantMap["SourceImageSize"].toMap()["Height"].toInt());

            _targetImageSize.setWidth(variantMap["TargetImageSize"].toMap()["Width"].toInt());
            _targetImageSize.setHeight(variantMap["TargetImageSize"].toMap()["Height"].toInt());

            break;
        }
    }
}

QVariantMap SelectionMap::toVariantMap() const
{
    std::vector<std::uint32_t> indices;
    std::vector<std::uint32_t> ranges;

    switch (_type)
    {
        case Type::Indexed:
        {
            ranges.reserve(_map.size() * 3);

            for (const auto& item : _map) {
                ranges.push_back(item.first);
                ranges.push_back(static_cast<std::uint32_t>(indices.size()));
                indices.insert(indices.end(), item.second.begin(), item.second.end());
                ranges.push_back(static_cast<std::uint32_t>(indices.size()));
            }

            break;
        }

        case Type::ImagePyramid:
            break;
    }

    const QVariantMap sourceImageSize{
        { "Width", QVariant::fromValue(_sourceImageSize.width()) },
        { "Height", QVariant::fromValue(_sourceImageSize.height()) }
    };

    const QVariantMap targetImageSize{
        { "Width", QVariant::fromValue(_targetImageSize.width()) },
        { "Height", QVariant::fromValue(_targetImageSize.height()) }
    };

    return {
        { "Type", QVariant::fromValue(static_cast<std::int32_t>(_type)) },
        { "NumberOfIndices", QVariant::fromValue(indices.size()) },
        { "Indices", rawDataToVariantMap((char*)indices.data(), indices.size() * sizeof(std::uint32_t), true) },
        { "IndicesSize", QVariant::fromValue(indices.size()) },
        { "Ranges", rawDataToVariantMap((char*)ranges.data(), ranges.size() * sizeof(std::uint32_t), true) },
        { "RangesSize", QVariant::fromValue(ranges.size()) },
        { "SourceImageSize", sourceImageSize },
        { "TargetImageSize", targetImageSize }
    };
}

LinkedData::LinkedData() :
    Serializable("LinkedData")
{
}

LinkedData::LinkedData(const Dataset<DatasetImpl>& sourceDataSet, const Dataset<DatasetImpl>& targetDataSet) :
    Serializable("LinkedData"),
    _sourceDataSet(sourceDataSet),
    _targetDataSet(targetDataSet)
{
    Q_ASSERT(_sourceDataSet.isValid());
    Q_ASSERT(_targetDataSet.isValid());
}

//LinkedData::LinkedData(const LinkedData& linkedData)
//{
//    *this = linkedData;
//}

const SelectionMap& LinkedData::getMapping() const
{
    return _mapping;
}

void LinkedData::setMapping(SelectionMap& mapping)
{
    _mapping = mapping;
}

void LinkedData::fromVariantMap(const QVariantMap& variantMap)
{
    variantMapMustContain(variantMap, "SourceDataset");
    variantMapMustContain(variantMap, "TargetDataset");
    variantMapMustContain(variantMap, "Mapping");

    _sourceDataSet.setDatasetId(variantMap["SourceDataset"].toString());
    _targetDataSet.setDatasetId(variantMap["TargetDataset"].toString());

    _mapping.fromVariantMap(variantMap["Mapping"].toMap());
}

QVariantMap LinkedData::toVariantMap() const
{
    return {
        { "SourceDataset", QVariant::fromValue(_sourceDataSet.getDatasetId()) },
        { "TargetDataset", QVariant::fromValue(_targetDataSet.getDatasetId()) },
        { "Mapping", _mapping.toVariantMap() }
    };
}

}
