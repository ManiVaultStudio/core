// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LinkedData.h"

#include "Set.h"

using namespace mv::util;

namespace mv
{

SelectionMap::SelectionMap(Type type /*= Indexed*/) :
    Serializable("SelectionMapping"),
    _type(type)
{
}

SelectionMap::SelectionMap(const QSize& sourceImageSize, const QSize& targetImageSize) :
    SelectionMap(Type::ImagePyramid)
{
    _sourceImageSize = sourceImageSize;
    _targetImageSize = targetImageSize;
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

void SelectionMap::fromVariantMap(const mv::VariantMap& variantMap)
{
    Serializable::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "Type");
    variantMapMustContain(variantMap, "SerializedMap");
    variantMapMustContain(variantMap, "SerializedMapSize");
    variantMapMustContain(variantMap, "SourceImageSize");
    variantMapMustContain(variantMap, "TargetImageSize");

    _type = static_cast<Type>(variantMap["Type"].toInt());

    auto SourceImageSizeMap = variantMap["SourceImageSize"].toMap();
    _sourceImageSize.setWidth(SourceImageSizeMap["Width"].toInt());
    _sourceImageSize.setHeight(SourceImageSizeMap["Height"].toInt());

    auto TargetImageSizeMap = variantMap["SourceImageSize"].toMap();
    _targetImageSize.setWidth(TargetImageSizeMap["Width"].toInt());
    _targetImageSize.setHeight(TargetImageSizeMap["Height"].toInt());

    std::vector<std::uint32_t> serializedMap;
    serializedMap.resize(static_cast<size_t>(variantMap["SerializedMapSize"].toInt()));
    populateDataBufferFromVariantMap(variantMap["SerializedMap"].toMap(), (char*)serializedMap.data());

    uint32_t key(0), size(0);
    for (auto it = serializedMap.begin(); it != serializedMap.end(); )
    {
        key = *it;
        it++;
        size = *it;
        it++;
        _map[key] = std::vector<std::uint32_t>(it, it + size);
        it += size;
    }
}

mv::VariantMap SelectionMap::toVariantMap() const
{
    auto variantMap = Serializable::toVariantMap();

    // serialize a std::map<std::uint32_t, std::vector<std::uint32_t>>;
    std::vector<std::uint32_t> serializedMap;

    for (const auto& [key, values] : _map)
    {
        serializedMap.push_back(key);
        serializedMap.push_back(static_cast<std::uint32_t>(values.size()));
        serializedMap.insert(serializedMap.end(), values.begin(), values.end());
    }

    const QVariantMap sourceImageSize{
        { "Width", QVariant::fromValue(_sourceImageSize.width()) },
        { "Height", QVariant::fromValue(_sourceImageSize.height()) }
    };

    const QVariantMap targetImageSize{
        { "Width", QVariant::fromValue(_targetImageSize.width()) },
        { "Height", QVariant::fromValue(_targetImageSize.height()) }
    };

    variantMap["Type"] = QVariant::fromValue(static_cast<std::int32_t>(_type));
    variantMap["SerializedMap"] = rawDataToVariantMap((char*)serializedMap.data(), serializedMap.size() * sizeof(std::uint32_t), true);
    variantMap["SerializedMapSize"] = QVariant::fromValue(serializedMap.size());
    variantMap["SourceImageSize"] = QVariant::fromValue(sourceImageSize);
    variantMap["TargetImageSize"] = QVariant::fromValue(targetImageSize);

    return variantMap;
}

LinkedData::LinkedData() :
    Serializable("LinkedData")
{
}

LinkedData::LinkedData(const Dataset<DatasetImpl>& sourceDataSet, const Dataset<DatasetImpl>& targetDataSet) :
    LinkedData()
{
    Q_ASSERT(sourceDataSet.isValid());
    Q_ASSERT(targetDataSet.isValid());

    _sourceDataSet = sourceDataSet;
    _targetDataSet = targetDataSet;
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

void LinkedData::fromVariantMap(const mv::VariantMap& variantMap)
{
    Serializable::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "SourceDataset");
    variantMapMustContain(variantMap, "TargetDataset");

    _sourceDataSet.setDatasetId(variantMap["SourceDataset"].toString());
    _targetDataSet.setDatasetId(variantMap["TargetDataset"].toString());

    _mapping.fromParentVariantMap(variantMap);
}

mv::VariantMap LinkedData::toVariantMap() const
{
    auto variantMap = Serializable::toVariantMap();

    variantMap["SourceDataset"] = QVariant::fromValue(_sourceDataSet.getDatasetId());
    variantMap["TargetDataset"] = QVariant::fromValue(_targetDataSet.getDatasetId());

    _mapping.insertIntoVariantMap(variantMap);

    return variantMap;

}

}
